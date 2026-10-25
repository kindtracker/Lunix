#include "Drivers/Hid.h"

static uint16_t OldState = 0xFFFF;
static uint16_t CurrentState = 0xFFFF;

void HidUpdate() {
  OldState = CurrentState;
  CurrentState = HID_REG;
}

uint16_t HidGetState() { return CurrentState; }
uint16_t HidGetDown() { return OldState & ~CurrentState & 0x0FFF; }
uint16_t HidGetUp() { return ~OldState & CurrentState & 0x0FFF; }
uint16_t HidGetHeld() { return ~CurrentState & 0x0FFF; }
uint16_t HidGetPressed() { return HidGetDown(); }
