#pragma once
#include "Arm9/Common.h"

typedef struct {
  char *Buffer;
  int Length;
  uint8_t *Screen;
  int SelectedX;
  int SelectedY;
} TerminalType;

void PutChar(TerminalType *Terminal, char Character);
void Puts(TerminalType *Terminal, const char *str);
void TerminalUpdate(TerminalType *Terminal);
void TerminalDraw(TerminalType *Terminal, int FrameCount);
void TerminalKeyboardDraw(TerminalType *Terminal);
