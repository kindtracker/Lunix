#include "Arm9/Common.h"
#include "Common/Ui.h"

#include "Console.h"

void PutChar(ConsoleType *Console, char Character) {
  Console->Buffer[Console->Length++] = Character;
}

void Puts(ConsoleType *Console, const char *String) {
  for (int i = 0; i < strlen(String); i++) {
    PutChar(Console, String[i]);
  }
}

void ConsoleDraw(ConsoleType *Console) {
  int X = 0;
  int Y = 0;

  for (int Idx = 0; Idx < Console->Length; Idx++) {
    char Character = Console->Buffer[Idx];
    if (Character == '\n') {
      X = 0;
      Y += 8;
    }
    DrawCharacter(Console->Screen, Character, X, Y, COLOR_STD_FONT,
                  COLOR_STD_BG);
    X += 8;
  }
}
