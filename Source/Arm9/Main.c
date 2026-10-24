#include "Arm9/Common.h"
#include "Common/Ui.h"
#include "Drivers/Mcu.h"
#include "FatFs/qff.h"
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

  ClearScreenF(true, true, COLOR_STD_BG);

  TerminalType Terminal = {"", 0, TopScreen};
  Puts(&Terminal, "Lunix v0.0.1\n");
  ShellPrompt(&Terminal);

  int FrameCount = 0;
  while (true) {
    if (!McuGetShellState()) {
      break;
    }

    TerminalDraw(&Terminal, FrameCount);

    FrameCount++;
  }

  fs_deinit();
  ClearScreenF(true, true, COLOR_STD_BG);
  SystemPoweroff();
}
