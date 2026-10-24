// Copyright 2013 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "Arm9/Common.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common/Ui.h"
#include "Font/font.h"

#define STRBUF_SIZE 512 // maximum size of the string buffer

void ClearScreen(uint8_t *screen, int color) {
  int width = (screen == TOP_SCREEN) ? SCREEN_WIDTH_TOP : SCREEN_WIDTH_BOT;
  if (color == COLOR_TRANSPARENT)
    color = COLOR_BLACK;
  for (int i = 0; i < (width * SCREEN_HEIGHT); i++) {
    *(screen++) = color >> 16;  // B
    *(screen++) = color >> 8;   // G
    *(screen++) = color & 0xFF; // R
  }
}

void ClearScreenF(bool clear_top, bool clear_bottom, int color) {
  if (clear_top)
    ClearScreen(TOP_SCREEN, color);
  if (clear_bottom)
    ClearScreen(BOT_SCREEN, color);
}

void DrawRectangle(uint8_t *screen, int x, int y, int width, int height,
                   int color) {
  for (int yy = 0; yy < height; yy++) {
    int xDisplacement = (x * BYTES_PER_PIXEL * SCREEN_HEIGHT);
    int yDisplacement = ((SCREEN_HEIGHT - (y + yy) - 1) * BYTES_PER_PIXEL);
    uint8_t *screenPos = screen + xDisplacement + yDisplacement;
    for (int xx = width - 1; xx >= 0; xx--) {
      *(screenPos + 0) = color >> 16;  // B
      *(screenPos + 1) = color >> 8;   // G
      *(screenPos + 2) = color & 0xFF; // R
      screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT;
    }
  }
}

void DrawCharacter(uint8_t *screen, int character, int x, int y, int color,
                   int bgcolor) {
  for (int yy = 0; yy < FONT_HEIGHT; yy++) {
    int xDisplacement = (x * BYTES_PER_PIXEL * SCREEN_HEIGHT);
    int yDisplacement = ((SCREEN_HEIGHT - (y + yy) - 1) * BYTES_PER_PIXEL);
    uint8_t *screenPos = screen + xDisplacement + yDisplacement;

    uint8_t charPos = font[character][yy];
    for (int xx = 0; xx < 8; xx++) {
      if ((charPos >> xx) & 1) {
        *(screenPos + 0) = color >> 16;  // B
        *(screenPos + 1) = color >> 8;   // G
        *(screenPos + 2) = color & 0xFF; // R
      } else if (bgcolor != COLOR_TRANSPARENT) {
        *(screenPos + 0) = bgcolor >> 16;  // B
        *(screenPos + 1) = bgcolor >> 8;   // G
        *(screenPos + 2) = bgcolor & 0xFF; // R
      }
      screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT;
    }
  }
}

void DrawString(uint8_t *screen, const char *str, int x, int y, int color,
                int bgcolor) {
  size_t max_len =
      (((screen == TOP_SCREEN) ? SCREEN_WIDTH_TOP : SCREEN_WIDTH_BOT) - x) /
      FONT_WIDTH;
  size_t len = (strlen(str) > max_len) ? max_len : strlen(str);
  for (size_t i = 0; i < len; i++)
    DrawCharacter(screen, str[i], x + i * FONT_WIDTH, y, color, bgcolor);
}

void DrawStringF(uint8_t *screen, int x, int y, int color, int bgcolor,
                 const char *format, ...) {
  char str[STRBUF_SIZE] = {0};
  va_list va;
  va_start(va, format);
  vsnprintf(str, STRBUF_SIZE, format, va);
  va_end(va);

  for (char *text = strtok(str, "\n"); text != NULL;
       text = strtok(NULL, "\n"), y += 10)
    DrawString(screen, text, x, y, color, bgcolor);
}

uint32_t GetDrawStringHeight(const char *str) {
  uint32_t height = FONT_HEIGHT;
  for (char *lf = strchr(str, '\n'); (lf != NULL); lf = strchr(lf + 1, '\n'))
    height += 10;
  return height;
}
