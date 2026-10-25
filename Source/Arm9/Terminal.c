#include "Terminal.h"
#include "Arm9/Common.h"
#include "Arm9/Ui.h"
#include "Drivers/Hid.h"

/*const char *KeyboardAbc[4] = {"1234567890", "qwertyuiop", "asdfghjkl-",
                              "zxcvbnm,./"};

const char *KeyboardAbcShift[4] = {"!@#$%^&*()", "QWERTYUIOP", "ASDFGHJKL_",
                                   "ZXCVBNM<>?"};

const char *KeyboardSymbol[4] = {"1234567890", "!@#$%^&*()", "~`|\\<>{}[]",
                                 "+=_:;\"',?-"};*/

#define KEYBOARD_COLS 9
#define KEYBOARD_ROWS 5
#define KEYBOARD_BORDER_SIZE 3

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
    {"Del", 8, 4, 1, 1, KEY_DELETE},  {"Enter", 9, 4, 2, 1, '\n'},
};

static const KeyboardKey KeyboardAbcShiftLayout[] = {
    {"1", 0, 0, 1, 1, '1'},           {"2", 1, 0, 1, 1, '2'},
    {"3", 2, 0, 1, 1, '3'},           {"4", 3, 0, 1, 1, '4'},
    {"5", 4, 0, 1, 1, '5'},           {"6", 5, 0, 1, 1, '6'},
    {"7", 6, 0, 1, 1, '7'},           {"8", 7, 0, 1, 1, '8'},
    {"9", 8, 0, 1, 1, '9'},           {"0", 9, 0, 1, 1, '0'},

    {"Q", 0, 1, 1, 1, 'Q'},           {"W", 1, 1, 1, 1, 'W'},
    {"E", 2, 1, 1, 1, 'E'},           {"R", 3, 1, 1, 1, 'R'},
    {"T", 4, 1, 1, 1, 'T'},           {"Y", 5, 1, 1, 1, 'Y'},
    {"U", 6, 1, 1, 1, 'U'},           {"I", 7, 1, 1, 1, 'I'},
    {"O", 8, 1, 1, 1, 'O'},           {"P", 9, 1, 1, 1, 'P'},

    {"A", 0, 2, 1, 1, 'A'},           {"S", 1, 2, 1, 1, 'S'},
    {"D", 2, 2, 1, 1, 'D'},           {"F", 3, 2, 1, 1, 'F'},
    {"G", 4, 2, 1, 1, 'G'},           {"H", 5, 2, 1, 1, 'H'},
    {"J", 6, 2, 1, 1, 'J'},           {"K", 7, 2, 1, 1, 'K'},
    {"L", 8, 2, 1, 1, 'L'},           {"_", 9, 2, 1, 1, '_'},

    {"Z", 0, 3, 1, 1, 'Z'},           {"X", 1, 3, 1, 1, 'X'},
    {"C", 2, 3, 1, 1, 'C'},           {"V", 3, 3, 1, 1, 'V'},
    {"B", 4, 3, 1, 1, 'B'},           {"N", 5, 3, 1, 1, 'N'},
    {"M", 6, 3, 1, 1, 'M'},           {"<", 7, 3, 1, 1, '<'},
    {">", 8, 3, 1, 1, '>'},           {"?", 9, 3, 1, 1, '?'},

    {"Shift", 0, 4, 2, 1, KEY_SHIFT}, {"?123", 2, 4, 2, 1, KEY_SYMBOLS},
    {"Tab", 4, 4, 1, 1, KEY_TAB},     {"Space", 5, 4, 3, 1, ' '},
    {"Del", 8, 4, 1, 1, KEY_DELETE},  {"Enter", 9, 4, 2, 1, '\n'},
};

static const KeyboardKey KeyboardSymbolLayout[] = {
    {"1", 0, 0, 1, 1, '1'},           {"2", 1, 0, 1, 1, '2'},
    {"3", 2, 0, 1, 1, '3'},           {"4", 3, 0, 1, 1, '4'},
    {"5", 4, 0, 1, 1, '5'},           {"6", 5, 0, 1, 1, '6'},
    {"7", 6, 0, 1, 1, '7'},           {"8", 7, 0, 1, 1, '8'},
    {"9", 8, 0, 1, 1, '9'},           {"0", 9, 0, 1, 1, '0'},

    {"!", 0, 1, 1, 1, '!'},           {"@", 1, 1, 1, 1, '@'},
    {"#", 2, 1, 1, 1, '#'},           {"$", 3, 1, 1, 1, '$'},
    {"%", 4, 1, 1, 1, '%'},           {"^", 5, 1, 1, 1, '^'},
    {"&", 6, 1, 1, 1, '&'},           {"*", 7, 1, 1, 1, '*'},
    {"(", 8, 1, 1, 1, '('},           {")", 9, 1, 1, 1, ')'},

    {"~", 0, 2, 1, 1, '~'},           {"`", 1, 2, 1, 1, '`'},
    {"|", 2, 2, 1, 1, '|'},           {"\\", 3, 2, 1, 1, '\\'},
    {"<", 4, 2, 1, 1, '<'},           {">", 5, 2, 1, 1, '>'},
    {"{", 6, 2, 1, 1, '{'},           {"}", 7, 2, 1, 1, '}'},
    {"[", 8, 2, 1, 1, '['},           {"]", 9, 2, 1, 1, ']'},

    {"+", 0, 3, 1, 1, '+'},           {"=", 1, 3, 1, 1, '='},
    {"_", 2, 3, 1, 1, '_'},           {":", 3, 3, 1, 1, ':'},
    {";", 4, 3, 1, 1, ';'},           {"\"", 5, 3, 1, 1, '"'},
    {"'", 6, 3, 1, 1, '\''},          {",", 7, 3, 1, 1, ','},
    {"?", 8, 3, 1, 1, '?'},           {"-", 9, 3, 1, 1, '-'},

    {"ABC", 0, 4, 2, 1, KEY_SYMBOLS}, {"?123", 2, 4, 2, 1, KEY_SYMBOLS},
    {"Tab", 4, 4, 1, 1, KEY_TAB},     {"Space", 5, 4, 3, 1, ' '},
    {"Del", 8, 4, 1, 1, KEY_DELETE},  {"Enter", 9, 4, 2, 1, '\n'},
};

KeyboardKey *SelectedKey;

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

    DrawCharacter(TopScreen, (unsigned char)Character, X, Y, COLOR_CAT_TEXT,
                  COLOR_CAT_BASE, 1);

    X += FONT_WIDTH;
  }

  int Color = (FrameCount % 512 > 256) ? COLOR_CAT_TEXT : COLOR_TRANSPARENT;

  X += FONT_WIDTH;
  DrawRectangle(TopScreen, X, Y, FONT_WIDTH, FONT_HEIGHT, Color);
}

void TerminalKeyboardDraw(TerminalType *Terminal) {
  int KeyWidth = SCREEN_WIDTH_BOTTOM / KEYBOARD_COLS;
  int KeyHeight = SCREEN_HEIGHT_BOTTOM / KEYBOARD_ROWS;

  const KeyboardKey *ActiveLayout;
  if (Terminal->Shift) {
    ActiveLayout = KeyboardAbcShiftLayout;
  } else if (Terminal->Symbol) {
    ActiveLayout = KeyboardSymbolLayout;
  } else {
    ActiveLayout = KeyboardAbcLayout;
  }

  for (unsigned int Idx = 0;
       Idx < sizeof(KeyboardAbcLayout) / sizeof(KeyboardAbcLayout[0]); Idx++) {
    const KeyboardKey *Key = &ActiveLayout[Idx];

    int X = Key->X * SCREEN_WIDTH_BOTTOM / KEYBOARD_COLS;
    int Y = Key->Y * SCREEN_HEIGHT_BOTTOM / KEYBOARD_ROWS;
    int Width = Key->Width * SCREEN_WIDTH_BOTTOM / KEYBOARD_COLS;
    int Height = Key->Height * SCREEN_HEIGHT_BOTTOM / KEYBOARD_ROWS;

    int IsKeySelected =
        Terminal->SelectedX == Key->X && Terminal->SelectedY == Key->Y;
    int BackgroundColor =
        IsKeySelected ? COLOR_CAT_SURFACE1 : COLOR_CAT_SURFACE0;

    if (IsKeySelected) {
      SelectedKey = Key;
    }

    DrawRectangle(BottomScreen, X + KEYBOARD_BORDER_SIZE,
                  Y + KEYBOARD_BORDER_SIZE, Width - KEYBOARD_BORDER_SIZE * 2,
                  Height - KEYBOARD_BORDER_SIZE * 2, BackgroundColor);

    int TextWidth = strlen(Key->Label) * FONT_WIDTH;
    int TextHeight = FONT_HEIGHT;
    int TextX = X + (Width - TextWidth) / 2;
    int TextY = Y + (Height - TextHeight) / 2;

    DrawString(BottomScreen, Key->Label, TextX, TextY, COLOR_CAT_TEXT,
               COLOR_TRANSPARENT, 1);
  }
}

void TerminalUpdate(TerminalType *Terminal) {
  int HidState = HidGetPressed();

  const KeyboardKey *ActiveLayout;
  unsigned int KeyCount =
      sizeof(KeyboardAbcLayout) / sizeof(KeyboardAbcLayout[0]);

  if (Terminal->Shift) {
    ActiveLayout = KeyboardAbcShiftLayout;
  } else if (Terminal->Symbol) {
    ActiveLayout = KeyboardSymbolLayout;
  } else {
    ActiveLayout = KeyboardAbcLayout;
  }

  int DeltaX = 0;
  int DeltaY = 0;

  if (HidState & KEY_UP) {
    DeltaY = -1;
  } else if (HidState & KEY_DOWN) {
    DeltaY = 1;
  } else if (HidState & KEY_LEFT) {
    DeltaX = -1;
  } else if (HidState & KEY_RIGHT) {
    DeltaX = 1;
  }

  if (DeltaX || DeltaY) {
    const KeyboardKey *CurrentKey = NULL;

    for (unsigned int Idx = 0; Idx < KeyCount; Idx++) {
      const KeyboardKey *Key = &ActiveLayout[Idx];

      if (Terminal->SelectedX == Key->X && Terminal->SelectedY == Key->Y) {
        CurrentKey = Key;
        break;
      }
    }

    if (CurrentKey == NULL)
      return;

    int StartX = CurrentKey->X;
    int StartY = CurrentKey->Y;

    if (DeltaX < 0)
      StartX = CurrentKey->X - 1;
    else if (DeltaX > 0)
      StartX = CurrentKey->X + CurrentKey->Width;
    else if (DeltaY < 0)
      StartY = CurrentKey->Y - 1;
    else
      StartY = CurrentKey->Y + CurrentKey->Height;

    for (int Step = 0; Step < KEYBOARD_COLS * KEYBOARD_ROWS; Step++) {
      int X = StartX + DeltaX * Step;
      int Y = StartY + DeltaY * Step;

      if (X < 0 || X >= KEYBOARD_COLS || Y < 0 || Y >= KEYBOARD_ROWS)
        break;

      for (unsigned int Idx = 0; Idx < KeyCount; Idx++) {
        const KeyboardKey *Key = &ActiveLayout[Idx];

        if (X >= Key->X && X < Key->X + Key->Width && Y >= Key->Y &&
            Y < Key->Y + Key->Height) {
          Terminal->SelectedX = Key->X;
          Terminal->SelectedY = Key->Y;
          return;
        }
      }
    }

    return;
  }

  if (HidState & KEY_A) {
    if (SelectedKey == NULL)
      return;

    int KeyCode = SelectedKey->KeyCode;

    if (KeyCode == KEY_SHIFT) {
      if (!Terminal->Symbol)
        Terminal->Shift = !Terminal->Shift;
    } else if (KeyCode == KEY_SYMBOLS) {
      Terminal->Shift = false;
      Terminal->Symbol = !Terminal->Symbol;
    } else if (KeyCode == KEY_TAB) {
      PutChar(Terminal, '\t');
    } else if (KeyCode == KEY_DELETE) {
      if (Terminal->Length > 0)
        Terminal->Buffer[--Terminal->Length] = '\0';
    } else {
      PutChar(Terminal, (char)KeyCode);

      if (Terminal->Shift)
        Terminal->Shift = false;
    }
  }
}
