#include <aurora_app.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sup.h"

extern const uint8_t SupBlob[], SupBlobEnd[];

int main() {
  const char *FileName = "/Meow.elf";

  printf("App: ELF = %s\n", FileName);

  printf("App: Loading Job Code\n");
  memcpy((void *)JOB_CODE, SupBlob, (size_t)(SupBlobEnd - SupBlob));
  memset(Job, 0, sizeof(JobParamsType));
  Job->Done = true;

  printf("App: Loading ELF Content and passing it to Job\n");
  FILE *FilePtr = fopen(FileName, "rb");
  if (!FilePtr) {
    printf("App: Failed to open ELF\n");
    hid_wait(KEY_A);
    return 1;
  }

  fseek(FilePtr, 0, SEEK_END);
  long FileSize = ftell(FilePtr);
  if (FileSize <= 0) {
    printf("App: Invalid ELF size\n");
    fclose(FilePtr);
    hid_wait(KEY_A);
    return 1;
  }
  fseek(FilePtr, 0, SEEK_SET);

  size_t Size = (size_t)FileSize;
  char *FileContent = malloc(Size);
  if (!FileContent) {
    printf("App: Failed to allocate ELF buffer\n");
    fclose(FilePtr);
    hid_wait(KEY_A);
    return 1;
  }

  size_t BytesRead = fread(FileContent, 1, Size, FilePtr);
  fclose(FilePtr);

  if (BytesRead != Size) {
    printf("App: Failed to read complete ELF\n");
    free(FileContent);
    hid_wait(KEY_A);
    return 1;
  }

  printf("Sup: Loading ELF\n");
  Job->Command = JOB_LOAD_ELF;
  Job->File = (uint32_t)FileContent;

  hid_wait(KEY_A);

  free(FileContent);

  return 0;
}
