#include "Arm9/Common.h"
#include "Common/Ui.h"
#include "Console.h"
#include "Drivers/Mcu.h"
#include "FatFs/qff.h"

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

  ConsoleType Console = {"", 0, TopScreen};

  ClearScreenF(true, true, COLOR_STD_BG);

  Puts(&Console, "Lunix v0.1.0\n");
  while (true) {
    ConsoleDraw(&Console);
    if (!(McuGetShellState() & 2)) {
      break;
    }
  }

  fs_deinit();
  ClearScreenF(true, true, COLOR_STD_BG);

  SystemPoweroff();
}
