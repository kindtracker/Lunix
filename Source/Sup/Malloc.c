#include <stddef.h>
#include <stdint.h>

#include "Sup.h"

typedef struct {
  uint32_t PageCount;
} SupAllocation;

uint32_t SupMalloc(size_t Size) {
  if (Size == 0)
    return 0;

  size_t RequiredPages =
      (Size + sizeof(SupAllocation) + PAGE_SIZE - 1) / PAGE_SIZE;

  if (RequiredPages > UINT32_MAX)
    return 0;

  void *Pointer = page_alloc();
  if (!Pointer)
    return 0;

  SupAllocation *Allocation = Pointer;
  Allocation->PageCount = (uint32_t)RequiredPages;

  for (size_t Idx = 1; Idx < RequiredPages; Idx++) {
    void *Page = page_alloc();

    if (!Page) {
      return 0;
    }
  }

  return (uint32_t)((uintptr_t)Pointer + sizeof(SupAllocation));
}

void SupFree(uint32_t Pointer) {
  if (!Pointer)
    return;

  SupAllocation *Allocation =
      (SupAllocation *)(Pointer - sizeof(SupAllocation));

  for (uint32_t Idx = 0; Idx < Allocation->PageCount; Idx++) {
    page_free(Allocation + Idx);
  }
}
