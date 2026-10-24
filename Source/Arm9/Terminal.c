#include "Terminal.h"
#include "Arm9/Common.h"
#include "Arm9/Ui.h"

void PutChar(TerminalType *Terminal, char Character) {
  Terminal->Buffer[Terminal->Length++] = Character;
}

void Puts(TerminalType *Terminal, const char *String) {
  for (int Idx = 0; String[Idx] != '\0'; Idx++) {
    PutChar(Terminal, String[Idx]);
  }
}

void TerminalDraw(TerminalType *Terminal, int FrameCount) {
  int X = 6;
  int Y = 6;

  for (int Idx = 0; Idx < Terminal->Length; Idx++) {
    char Character = Terminal->Buffer[Idx];

    if (Character == '\n') {
      X = 6;
      Y += FONT_HEIGHT;
      continue;
    }

    DrawCharacter(Terminal->Screen, (unsigned char)Character, X, Y, COLOR_WHITE,
                  COLOR_BLACK, 1);

    X += FONT_WIDTH;
  }

  int Color = (FrameCount % 512 > 256) ? COLOR_WHITE : COLOR_BLACK;

  X += FONT_WIDTH;
  DrawRectangle(Terminal->Screen, X, Y, FONT_WIDTH, FONT_HEIGHT, Color);
}
