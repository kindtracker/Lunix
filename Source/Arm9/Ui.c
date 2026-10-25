#include "Arm9/Ui.h"
#include "Arm9/Common.h"
#include "Font/Font.h"
#include <stdio.h>

#define STRBUF_SIZE 512

void DrawPixel(uint8_t *Screen, int X, int Y, int Color) {
  if (Screen == NULL) {
    return;
  }

  if (Color == COLOR_TRANSPARENT) {
    return;
  }

  int Width = (Screen == TopScreen) ? SCREEN_WIDTH_TOP : SCREEN_WIDTH_BOTTOM;

  if (X < 0 || X >= Width || Y < 0 || Y >= SCREEN_HEIGHT) {
    return;
  }

  int XDisplacement = X * BYTES_PER_PIXEL * SCREEN_HEIGHT;
  int YDisplacement = (SCREEN_HEIGHT - Y - 1) * BYTES_PER_PIXEL;

  uint8_t *ScreenPosition = Screen + XDisplacement + YDisplacement;

  ScreenPosition[0] = (Color >> 16) & 0xFF;
  ScreenPosition[1] = (Color >> 8) & 0xFF;
  ScreenPosition[2] = Color & 0xFF;
}

void ClearScreen(uint8_t *Screen) {
  if (Screen == NULL) {
    return;
  }

  int Width = (Screen == TopScreen) ? SCREEN_WIDTH_TOP : SCREEN_WIDTH_BOTTOM;

  for (int X = 0; X < Width; X++) {
    for (int Y = 0; Y < SCREEN_HEIGHT; Y++) {
      DrawPixel(Screen, X, Y, COLOR_CAT_BASE);
    }
  }
}

void ClearAllScreen(void) {
  ClearScreen(TopScreen);
  ClearScreen(BottomScreen);
}

void DrawRectangle(uint8_t *Screen, int RectangleX, int RectangleY, int Width,
                   int Height, int Color) {
  if (Screen == NULL || Width <= 0 || Height <= 0) {
    return;
  }

  for (int Y = 0; Y < Height; Y++) {
    for (int X = 0; X < Width; X++) {
      DrawPixel(Screen, RectangleX + X, RectangleY + Y, Color);
    }
  }
}

void DrawCharacter(uint8_t *Screen, int Character, int CharacterX,
                   int CharacterY, int FontColor, int BackgroundColor,
                   int FontSize) {
  if (Screen == NULL || FontSize <= 0) {
    return;
  }

  /* Font contains U+0000 through U+007F only. */
  if (Character < 0 || Character >= 128) {
    Character = '?';
  }

  for (int Y = 0; Y < FONT_HEIGHT; Y++) {
    uint8_t Row = Font[Character][Y];

    for (int X = 0; X < FONT_WIDTH; X++) {
      bool Bit = (Row & (1u << X)) != 0;

      if (!Bit && BackgroundColor == COLOR_TRANSPARENT) {
        continue;
      }

      int Color = Bit ? FontColor : BackgroundColor;

      DrawRectangle(Screen, CharacterX + X * FontSize,
                    CharacterY + Y * FontSize, FontSize, FontSize, Color);
    }
  }
}

void DrawString(uint8_t *Screen, const char *String, int X, int Y,
                int FontColor, int BackgroundColor, int FontSize) {
  if (Screen == NULL || String == NULL || FontSize <= 0) {
    return;
  }

  int ScreenWidth =
      (Screen == TopScreen) ? SCREEN_WIDTH_TOP : SCREEN_WIDTH_BOTTOM;

  if (X < 0 || X >= ScreenWidth) {
    return;
  }

  int CharacterWidth = FONT_WIDTH * FontSize;

  size_t MaxLength = (size_t)(ScreenWidth - X) / CharacterWidth;
  size_t StringLength = strlen(String);
  size_t Length = StringLength < MaxLength ? StringLength : MaxLength;

  for (size_t Idx = 0; Idx < Length; Idx++) {
    DrawCharacter(Screen, (unsigned char)String[Idx],
                  X + (int)Idx * CharacterWidth, Y, FontColor, BackgroundColor,
                  FontSize);
  }
}

void DrawStringF(uint8_t *Screen, int X, int Y, int FontColor,
                 int BackgroundColor, int FontSize, const char *Format, ...) {
  if (Screen == NULL || Format == NULL || FontSize <= 0) {
    return;
  }

  char String[STRBUF_SIZE];

  va_list Va;
  va_start(Va, Format);
  vsnprintf(String, sizeof(String), Format, Va);
  va_end(Va);

  char *Text = strtok(String, "\n");

  while (Text != NULL) {
    DrawString(Screen, Text, X, Y, FontColor, BackgroundColor, FontSize);

    Y += 10 * FontSize;
    Text = strtok(NULL, "\n");
  }
}

uint32_t GetDrawStringHeight(const char *String) {
  if (String == NULL || *String == '\0') {
    return 0;
  }

  uint32_t Height = FONT_HEIGHT;

  for (const char *Char = String; *Char != '\0'; Char++) {
    if (*Char == '\n') {
      Height += 10;
    }
  }

  return Height;
}
