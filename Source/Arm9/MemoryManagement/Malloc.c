#include "MemoryManagement/Malloc.h"
#include "Arm9/Common.h"

#define TOTAL_PAGES ((MEMORY_END - MEMORY_START) / PAGE_SIZE)
#define BITMAP_SIZE ((TOTAL_PAGES + 7) / 8)

static uint8_t Bitmap[BITMAP_SIZE];

void MallocInitialize(void) { memset(Bitmap, 0, sizeof(Bitmap)); }

void *Malloc(size_t Size) {
  if (Size == 0) {
    return NULL;
  }

  if (Size > SIZE_MAX - sizeof(size_t)) {
    return NULL;
  }

  Size += sizeof(size_t);

  size_t PageCount = Size / PAGE_SIZE + (Size % PAGE_SIZE != 0);

  if (PageCount > TOTAL_PAGES) {
    return NULL;
  }

  size_t FoundFreePages = 0;
  size_t StartPage = 0;

  for (size_t Page = 0; Page < TOTAL_PAGES; Page++) {
    size_t BitmapIdx = Page / 8;
    uint8_t BitmapBit = (uint8_t)(1u << (Page % 8));

    if ((Bitmap[BitmapIdx] & BitmapBit) == 0) {
      if (FoundFreePages == 0) {
        StartPage = Page;
      }

      FoundFreePages++;

      if (FoundFreePages == PageCount) {
        for (size_t Idx = StartPage; Idx < StartPage + PageCount; Idx++) {
          Bitmap[Idx / 8] |= (uint8_t)(1u << (Idx % 8));
        }

        uintptr_t Address = (uintptr_t)MEMORY_START + StartPage * PAGE_SIZE;

        *(size_t *)Address = PageCount;

        return (void *)(Address + sizeof(size_t));
      }
    } else {
      FoundFreePages = 0;
    }
  }

  return NULL;
}

void *Realloc(void *Pointer, size_t NewSize) {
  if (Pointer == NULL) {
    return Malloc(NewSize);
  }

  if (NewSize == 0) {
    Free(Pointer);
    return NULL;
  }

  void *NewPointer = Malloc(NewSize);

  if (NewPointer == NULL) {
    return NULL;
  }

  size_t OldSize =
      (*(size_t *)((uintptr_t)Pointer - sizeof(size_t))) * PAGE_SIZE -
      sizeof(size_t);

  size_t CopySize = OldSize < NewSize ? OldSize : NewSize;

  Memcpy(NewPointer, Pointer, CopySize);
  Free(Pointer);

  return NewPointer;
}

void Free(void *Pointer) {
  if (Pointer == NULL) {
    return;
  }

  uintptr_t Address = (uintptr_t)Pointer;

  uintptr_t HeaderAddress = Address - sizeof(size_t);

  if (HeaderAddress < (uintptr_t)MEMORY_START ||
      HeaderAddress >= (uintptr_t)MEMORY_END) {
    return;
  }

  size_t StartPage = (HeaderAddress - (uintptr_t)MEMORY_START) / PAGE_SIZE;

  if (Address !=
      (uintptr_t)MEMORY_START + StartPage * PAGE_SIZE + sizeof(size_t)) {
    return;
  }

  size_t PageCount = *(size_t *)HeaderAddress;

  if (PageCount == 0 || PageCount > TOTAL_PAGES - StartPage) {
    return;
  }

  for (size_t Idx = StartPage; Idx < StartPage + PageCount; Idx++) {
    Bitmap[Idx / 8] &= (uint8_t)~(1u << (Idx % 8));
  }
}
