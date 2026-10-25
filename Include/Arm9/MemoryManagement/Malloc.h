#pragma once

#define PAGE_SIZE 4096

void MallocInitialize();
void *Malloc(size_t Size);
void *Realloc(size_t NewSize);
void Free(void *Pointer);
