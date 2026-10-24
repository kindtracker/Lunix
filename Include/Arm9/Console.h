#pragma once

typedef struct {
  char *Buffer;
  int Length;
  uint8_t *Screen;
} ConsoleType;

void PutChar(ConsoleType *Console, char Character);
void Puts(ConsoleType *Console, const char *str);
void ConsoleDraw(ConsoleType *Console);
