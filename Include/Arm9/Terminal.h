#pragma once
#include "Arm9/Common.h"

typedef struct {
  char *Buffer;
  int Length;
  uint8_t *Screen;
} TerminalType;

void PutChar(TerminalType *Terminal, char Character);
void Puts(TerminalType *Terminal, const char *str);
void TerminalDraw(TerminalType *Terminal);
