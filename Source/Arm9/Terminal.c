#include "Terminal.h"
#include "Arm9/Common.h"
#include "Arm9/Ui.h"

/*const char *KeyboardAbc[4] = {"1234567890", "qwertyuiop", "asdfghjkl-",
                              "zxcvbnm,./"};

const char *KeyboardAbcShift[4] = {"!@#$%^&*()", "QWERTYUIOP", "ASDFGHJKL_",
                                   "ZXCVBNM<>?"};

const char *KeyboardSymbol[4] = {"1234567890", "!@#$%^&*()", "~`|\\<>{}[]",
                                 "+=_:;\"',?-"};*/

typedef struct {
  const char *Label;
  int X, Y;
  int Width, Height;
  int KeyCode;
} KeyboardKey;

static const KeyboardKey KeyboardAbcLayout[] = {
    {"1", 0, 0, 1, 1, '1'},           {"2", 1, 0, 1, 1, '2'},
    {"3", 2, 0, 1, 1, '3'},

    {"Shift", 0, 4, 2, 1, KEY_SHIFT}, {"?123", 2, 4, 2, 1, KEY_SYMBOLS},
    {"Tab", 4, 4, 1, 1, KEY_TAB},     {"Space", 5, 4, 3, 1, ' '},
    {"Del", 8, 4, 1, 1, KEY_DELETE},  {"Enter", 9, 4, 2, 1, KEY_ENTER},
};

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

    DrawCharacter(TopScreen, (unsigned char)Character, X, Y, COLOR_WHITE,
                  COLOR_BLACK, 1);

    X += FONT_WIDTH;
  }

  int Color = (FrameCount % 512 > 256) ? COLOR_WHITE : COLOR_BLACK;

  X += FONT_WIDTH;
  DrawRectangle(TopScreen, X, Y, FONT_WIDTH, FONT_HEIGHT, Color);
}

void TerminalKeyboardDraw(TerminalType *Terminal) {
  int RowCount = sizeof(KeyboardAbc) / sizeof(KeyboardAbc[0]);
  for (int RowIndex = 0; RowIndex < RowCount; RowIndex++) {
    const char *Row = KeyboardAbc[RowIndex];
    int RowLength = strlen(Row);

    for (int Idx = 0; Row[Idx] != '\0'; Idx++) {
      char Character = Row[Idx];

      int X = Idx * (SCREEN_WIDTH_BOTTOM / RowLength);
      int Y = RowIndex * (SCREEN_HEIGHT_BOTTOM / RowCount - 2);

      DrawCharacter(BottomScreen, (unsigned char)Character, X, Y, COLOR_WHITE,
                    COLOR_BLACK, 1);
    }
  }
}
