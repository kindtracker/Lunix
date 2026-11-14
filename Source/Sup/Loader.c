#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sup.h"

void SupLoadProgram() {
  Elf64_Ehdr *Eh = (Elf64_Ehdr *)Job->File;

  if (memcmp(Eh->e_ident, ELFMAG, SELFMAG) != 0 ||
      Eh->e_machine != EM_AARCH64) {
    Job->Result = NOT_OK;
    return;
  }

  Elf64_Phdr *Phdrs = (Elf64_Phdr *)(Job->File + Eh->e_phoff);

  uint64_t MinAddress = UINT64_MAX;
  uint64_t MaxAddress = 0;

  for (int Idx = 0; Idx < Eh->e_phnum; Idx++) {
    Elf64_Phdr *Ph = &Phdrs[Idx];

    if (Ph->p_type != PT_LOAD)
      continue;

    uint64_t Address = Ph->p_vaddr & ~0xfffULL;
    uint64_t End = (Ph->p_vaddr + Ph->p_memsz + 0xfff) & ~0xfffULL;

    if (MinAddress > Address)
      MinAddress = Address;

    if (MaxAddress < End)
      MaxAddress = End;
  }

  uc_err Error =
      uc_mem_map(Unicorn, MinAddress, MaxAddress - MinAddress, UC_PROT_ALL);

  if (Error != UC_ERR_OK) {
    fprintf(stderr, "[Lunix] failed to map elf segment: %s\n",
            uc_strerror(Error));
    free(Job->File);
    return -1;
  }

  for (int Idx = 0; Idx < Eh->e_phnum; Idx++) {
    Elf64_Phdr *Ph = &Phdrs[Idx];

    if (Ph->p_type != PT_LOAD)
      continue;

    Error = uc_mem_write(Unicorn, Ph->p_vaddr, Job->File + Ph->p_offset,
                         Ph->p_filesz);

    if (Error != UC_ERR_OK) {
      fprintf(stderr, "[Lunix] failed to write elf segment: %s\n",
              uc_strerror(Error));
      free(Job->File);
      return -1;
    }

    if (Ph->p_memsz > Ph->p_filesz) {
      uint8_t *Zero = calloc(1, Ph->p_memsz - Ph->p_filesz);

      if (!Zero) {
        free(Job->File);
        return -1;
      }

      Error = uc_mem_write(Unicorn, Ph->p_vaddr + Ph->p_filesz, Zero,
                           Ph->p_memsz - Ph->p_filesz);

      free(Zero);

      if (Error != UC_ERR_OK) {
        fprintf(stderr, "[Lunix] failed to clear elf bss: %s\n",
                uc_strerror(Error));
        free(Job->File);
        return -1;
      }
    }
  }

  *Entry = Eh->e_entry;

  free(Job->File);
  return 0;
}
