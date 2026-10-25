#include "Arm9/Common.h"
#include "Arm9/Ui.h"
#include "Drivers/Hid.h"
#include "Drivers/Mcu.h"
#include "Shell/Shell.h"
#include "Terminal.h"

uint8_t *TopScreen, *BottomScreen;

void Main(int argc, char **argv) {
  if (argc >= 2) {
    uint8_t **FrameBuffer = (uint8_t **)(void *)argv[1];
    TopScreen = FrameBuffer[0];
    BottomScreen = FrameBuffer[2];
  } else {
    TopScreen = (uint8_t *)(*(uint32_t *)0x23FFFE00);
    BottomScreen = (uint8_t *)(*(uint32_t *)0x23FFFE08);
  }

  ClearAllScreen();

  TerminalType Terminal = {"", 0, TopScreen, 0, 0, false, false};
  Puts(&Terminal, "Lunix v0.0.1\n");
  ShellPrompt(&Terminal);

  int FrameCount = 0;
  while (true) {
    HidUpdate();

    int Keys = HidGetPressed();
    if (!McuGetShellState() || Keys & KEY_L) {
      break;
    }

    TerminalUpdate(&Terminal);
    TerminalDraw(&Terminal, FrameCount);
    TerminalKeyboardDraw(&Terminal);

    FrameCount++;
  }

  ClearAllScreen();
  SystemPoweroff();
}
