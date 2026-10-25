#pragma once
#include "Arm9/Common.h"

#define HID_REG (*(volatile uint16_t *)(0x10146000))

#define KEY_A (1u << 0)
#define KEY_B (1u << 1)
#define KEY_SELECT (1u << 2)
#define KEY_START (1u << 3)
#define KEY_RIGHT (1u << 4)
#define KEY_LEFT (1u << 5)
#define KEY_UP (1u << 6)
#define KEY_DOWN (1u << 7)
#define KEY_R (1u << 8)
#define KEY_L (1u << 9)
#define KEY_X (1u << 10)
#define KEY_Y (1u << 11)

void HidUpdate();

uint16_t HidGetState();
uint16_t HidGetDown();
uint16_t HidGetUp();
uint16_t HidGetHeld();
uint16_t HidGetPressed();
