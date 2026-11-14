#include <stddef.h>

void *memcpy(void *Destination, const void *Source, size_t Size) {
  unsigned char *Dest = Destination;
  const unsigned char *Src = Source;

  for (size_t Idx = 0; Idx < Size; Idx++)
    Dest[Idx] = Src[Idx];

  return Destination;
}

void *memset(void *Destination, int Value, size_t Size) {
  unsigned char *Dest = Destination;

  for (size_t Idx = 0; Idx < Size; Idx++)
    Dest[Idx] = (unsigned char)Value;

  return Destination;
}

int memcmp(const void *First, const void *Second, size_t Size) {
  const unsigned char *A = First;
  const unsigned char *B = Second;

  for (size_t Idx = 0; Idx < Size; Idx++) {
    if (A[Idx] != B[Idx])
      return A[Idx] - B[Idx];
  }

  return 0;
}
