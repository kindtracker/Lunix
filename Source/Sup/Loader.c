#include <elf.h>
#include <stdint.h>
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

  if (MinAddress == UINT64_MAX || MaxAddress <= MinAddress) {
    Job->Result = NOT_OK;
    return;
  }

  uint64_t Size = MaxAddress - MinAddress;
  if (Size > UINT32_MAX) {
    Job->Result = NOT_OK;
    return;
  }

  uint32_t PhysicalAddress = SupMalloc((uint32_t)Size);
  if (!PhysicalAddress) {
    Job->Result = NOT_OK;
    return;
  }

  mmu_map(MinAddress, PhysicalAddress, (uint32_t)Size, MAP_USER);
  for (int Idx = 0; Idx < Eh->e_phnum; Idx++) {
    Elf64_Phdr *Ph = &Phdrs[Idx];
    if (Ph->p_type != PT_LOAD)
      continue;

    uint32_t SegmentPhysicalAddress =
        PhysicalAddress + (uint32_t)(Ph->p_vaddr - MinAddress);
    uint8_t *Segment = (uint8_t *)SegmentPhysicalAddress;
    memcpy(Segment, Job->File + Ph->p_offset, Ph->p_filesz);

    if (Ph->p_memsz > Ph->p_filesz) {
      memset(Segment + Ph->p_filesz, 0, Ph->p_memsz - Ph->p_filesz);
    }
  }

  Job->Entry = Eh->e_entry;
  Job->Result = OK;
  return;
}
