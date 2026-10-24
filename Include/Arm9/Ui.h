// Copyright 2013 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include "Arm9/Common.h"

#define BYTES_PER_PIXEL 3
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH_TOP 400
#define SCREEN_WIDTH_BOTTOM 320
#define SCREEN_HEIGHT_TOP 240
#define SCREEN_HEIGHT_BOTTOM 240
#define FONT_WIDTH 8
#define FONT_HEIGHT 8

#define RGB(R, G, B) ((R) << 24 | (B) << 16 | (G) << 8 | (R))

#define COLOR_BLACK RGB(0x00, 0x00, 0x00)
#define COLOR_WHITE RGB(0xFF, 0xFF, 0xFF)
#define COLOR_GREY RGB(0x80, 0x80, 0x80)

#define COLOR_RED RGB(0xFF, 0x00, 0x00)
#define COLOR_GREEN RGB(0x00, 0xFF, 0x00)
#define COLOR_BLUE RGB(0x00, 0x00, 0xFF)
#define COLOR_YELLOW RGB(0xFF, 0xFF, 0x00)
#define COLOR_CYAN RGB(0xFF, 0x00, 0xFF)
#define COLOR_ORANGE RGB(0xFF, 0xA5, 0x00)

#define COLOR_BRIGHTRED RGB(0xFF, 0x30, 0x30)
#define COLOR_DARKRED RGB(0x80, 0x00, 0x00)
#define COLOR_BRIGHTYELLOW RGB(0xFF, 0xFF, 0x30)
#define COLOR_BRIGHTGREEN RGB(0x30, 0xFF, 0x30)
#define COLOR_BRIGHTBLUE RGB(0x30, 0x30, 0xFF)

#define COLOR_TINTEDBLUE RGB(0x60, 0x60, 0x80)
#define COLOR_TINTEDYELLOW RGB(0xD0, 0xD0, 0x60)
#define COLOR_TINTEDGREEN RGB(0x70, 0x80, 0x70)
#define COLOR_LIGHTGREY RGB(0xB0, 0xB0, 0xB0)
#define COLOR_DARKGREY RGB(0x50, 0x50, 0x50)
#define COLOR_DARKESTGREY RGB(0x20, 0x20, 0x20)

#define COLOR_TRANSPARENT                                                      \
  RGB(0xFF, 0x00, 0xEF) // otherwise known as 'super fuchsia'

extern uint8_t *TopScreen, *BottomScreen;

void DrawPixel(uint8_t *Screen, int X, int Y, int Color);
void ClearScreen(uint8_t *Screen);
void ClearAllScreen();
void DrawRectangle(uint8_t *Screen, int RectangleX, int RectangleY, int Width,
                   int Height, int Color);
void DrawCharacter(uint8_t *Screen, int Character, int CharacterX,
                   int CharacterY, int FontColor, int BackgroundColor,
                   int FontSize);
void DrawString(uint8_t *Screen, const char *String, int X, int Y,
                int FontColor, int BackgroundColor, int FontSize);
void DrawStringF(uint8_t *Screen, int X, int Y, int FontColor,
                 int BackgroundColor, int FontSize, const char *Format, ...);
