#include "Terminal.h"
#include "Arm9/Common.h"
#include "Arm9/Ui.h"

/*const char *KeyboardAbc[4] = {"1234567890", "qwertyuiop", "asdfghjkl-",
                              "zxcvbnm,./"};

const char *KeyboardAbcShift[4] = {"!@#$%^&*()", "QWERTYUIOP", "ASDFGHJKL_",
                                   "ZXCVBNM<>?"};

const char *KeyboardSymbol[4] = {"1234567890", "!@#$%^&*()", "~`|\\<>{}[]",
                                 "+=_:;\"',?-"};*/

#define KEYBOARD_COLS 9
#define KEYBOARD_ROWS 5

typedef struct {
  const char *Label;
  int X, Y;
  int Width, Height;
  int KeyCode;
} KeyboardKey;

typedef enum {
  KEY_SHIFT,
  KEY_TAB,
  KEY_DELETE,
  KEY_SYMBOLS,
  KEY_ENTER,
} KeyCodeType;

static const KeyboardKey KeyboardAbcLayout[] = {
    {"1", 0, 0, 1, 1, '1'},           {"2", 1, 0, 1, 1, '2'},
    {"3", 2, 0, 1, 1, '3'},           {"4", 3, 0, 1, 1, '4'},
    {"5", 4, 0, 1, 1, '5'},           {"6", 5, 0, 1, 1, '6'},
    {"7", 6, 0, 1, 1, '7'},           {"8", 7, 0, 1, 1, '8'},
    {"9", 8, 0, 1, 1, '9'},           {"0", 9, 0, 1, 1, '0'},

    {"q", 0, 1, 1, 1, 'q'},           {"w", 1, 1, 1, 1, 'w'},
    {"e", 2, 1, 1, 1, 'e'},           {"r", 3, 1, 1, 1, 'r'},
    {"t", 4, 1, 1, 1, 't'},           {"y", 5, 1, 1, 1, 'y'},
    {"u", 6, 1, 1, 1, 'u'},           {"i", 7, 1, 1, 1, 'i'},
    {"o", 8, 1, 1, 1, 'o'},           {"p", 9, 1, 1, 1, 'p'},

    {"a", 0, 2, 1, 1, 'a'},           {"s", 1, 2, 1, 1, 's'},
    {"d", 2, 2, 1, 1, 'd'},           {"f", 3, 2, 1, 1, 'f'},
    {"g", 4, 2, 1, 1, 'g'},           {"h", 5, 2, 1, 1, 'h'},
    {"j", 6, 2, 1, 1, 'j'},           {"k", 7, 2, 1, 1, 'k'},
    {"l", 8, 2, 1, 1, 'l'},           {"-", 9, 2, 1, 1, '-'},

    {"z", 0, 3, 1, 1, 'z'},           {"x", 1, 3, 1, 1, 'x'},
    {"c", 2, 3, 1, 1, 'c'},           {"v", 3, 3, 1, 1, 'v'},
    {"b", 4, 3, 1, 1, 'b'},           {"n", 5, 3, 1, 1, 'n'},
    {"m", 6, 3, 1, 1, 'm'},           {",", 7, 3, 1, 1, ','},
    {".", 8, 3, 1, 1, '.'},           {"/", 9, 3, 1, 1, '/'},

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
  int KeyWidth = SCREEN_WIDTH_BOTTOM / KEYBOARD_COLS;
  int KeyHeight = SCREEN_HEIGHT_BOTTOM / KEYBOARD_ROWS;

  for (unsigned int Idx = 0;
       Idx < sizeof(KeyboardAbcLayout) / sizeof(KeyboardAbcLayout[0]); Idx++) {
    const KeyboardKey *Key = &KeyboardAbcLayout[Idx];

    int X = Key->X * SCREEN_WIDTH_BOTTOM / KEYBOARD_COLS;
    int Y = Key->Y * SCREEN_HEIGHT_BOTTOM / KEYBOARD_ROWS;
    int Width = Key->Width * SCREEN_WIDTH_BOTTOM / KEYBOARD_COLS;
    int Height = Key->Height * SCREEN_HEIGHT_BOTTOM / KEYBOARD_ROWS;

    DrawRectangle(BottomScreen, X, Y, Width, Height, COLOR_BLACK);

    int TextWidth = strlen(Key->Label) * FONT_WIDTH;
    int TextHeight = FONT_HEIGHT;
    int TextX = X + (Width - TextWidth) / 2;
    int TextY = Y + (Height - TextHeight) / 2;

    DrawString(BottomScreen, Key->Label, TextX, TextY, COLOR_WHITE, COLOR_BLACK,
               1);
  }
}
