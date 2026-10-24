#include "Arm9/Common.h"
#include "Arm9/Ui.h"

#include "Terminal.h"

void PutChar(TerminalType *Terminal, char Character) {
  Terminal->Buffer[Terminal->Length++] = Character;
}

void Puts(TerminalType *Terminal, const char *String) {
  for (int i = 0; i < strlen(String); i++) {
    PutChar(Terminal, String[i]);
  }
}

void TerminalDraw(TerminalType *Terminal, int FrameCount) {
  int X = 6;
  int Y = 6;

  for (int Idx = 0; Idx < Terminal->Length; Idx++) {
    char Character = Terminal->Buffer[Idx];
    if (Character == '\n') {
      X = 6;
      Y += 8;
      continue;
    }
    DrawCharacter(Terminal->Screen, Character, X, Y, COLOR_WHITE, COLOR_BLACK,
                  8);
    X += 8;
  }

  int Color = FrameCount % 1024 > 512 ? COLOR_WHITE : COLOR_BLACK;
  X += 8;
  DrawRectangle(Terminal->Screen, X, Y, 8, 8, Color);
}
