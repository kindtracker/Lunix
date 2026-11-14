#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sup.h"

void SupLoadProgram() {
  Elf32_Ehdr *Eh = (Elf32_Ehdr *)(u32)Job->File;

  if (memcmp(Eh->e_ident, ELFMAG, SELFMAG) != 0 ||
      Eh->e_ident[EI_CLASS] != ELFCLASS32 || Eh->e_machine != EM_ARM) {
    Job->Result = NOT_OK;
    return;
  }

  Elf32_Phdr *Phdrs = (Elf32_Phdr *)((u32)Job->File + Eh->e_phoff);

  uint32_t MinAddress = UINT32_MAX;
  uint32_t MaxAddress = 0;

  for (int Idx = 0; Idx < Eh->e_phnum; Idx++) {
    Elf32_Phdr *Ph = &Phdrs[Idx];

    if (Ph->p_type != PT_LOAD)
      continue;

    uint32_t Address = Ph->p_vaddr & ~0xfff;
    uint32_t End = (Ph->p_vaddr + Ph->p_memsz + 0xfff) & ~0xfff;

    if (MinAddress > Address)
      MinAddress = Address;

    if (MaxAddress < End)
      MaxAddress = End;
  }

  if (MinAddress == UINT32_MAX || MaxAddress <= MinAddress) {
    Job->Result = NOT_OK;
    return;
  }

  uint32_t Size = MaxAddress - MinAddress;

  uint32_t PhysicalAddress = SupMalloc(Size);
  if (!PhysicalAddress) {
    Job->Result = NOT_OK;
    return;
  }

  mmu_map(MinAddress, PhysicalAddress, Size, MAP_USER);

  for (int Idx = 0; Idx < Eh->e_phnum; Idx++) {
    Elf32_Phdr *Ph = &Phdrs[Idx];

    if (Ph->p_type != PT_LOAD)
      continue;

    uint32_t SegmentPhysicalAddress =
        PhysicalAddress + (Ph->p_vaddr - MinAddress);

    uint8_t *Segment = (uint8_t *)SegmentPhysicalAddress;

    memcpy(Segment, (void *)(u32)(Job->File + Ph->p_offset), Ph->p_filesz);

    if (Ph->p_memsz > Ph->p_filesz) {
      memset(Segment + Ph->p_filesz, 0, Ph->p_memsz - Ph->p_filesz);
    }
  }

  Job->Entry = Eh->e_entry;
  Job->Result = OK;
}
