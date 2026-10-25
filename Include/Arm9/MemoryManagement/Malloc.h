#pragma once
#include "Arm9/Common.h"

extern uint8_t MemoryStart[];
extern uint8_t MemoryEnd[];

#define MEMORY_START ((uintptr_t)MemoryStart)
#define MEMORY_END ((uintptr_t)MemoryEnd)
#define TOTAL_PAGES ((MEMORY_END - MEMORY_START) / PAGE_SIZE)
#define BITMAP_SIZE 32761
#define PAGE_SIZE 4096

void MallocInitialize();
void *Malloc(size_t Size);
void *Realloc(void *Pointer, size_t NewSize);
void Free(void *Pointer);
