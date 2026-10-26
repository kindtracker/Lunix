#include "Arm9/Common.h"
#include "Arm9/Ui.h"
#include "Drivers/Hid.h"
#include "Drivers/Mcu.h"
#include "MemoryManagement/Malloc.h"
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

  char *Buffer = Malloc(256);

  TerminalType Terminal = {.Buffer = Buffer,
                           .BufferLength = 256,
                           .Length = 0,
                           .Screen = TopScreen,
                           .SelectedX = 0,
                           .SelectedY = 0,
                           .Shift = false,
                           .Symbol = false};
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

    Printf(&Terminal, "\nLength: %d", Terminal.Length);

    FrameCount++;
  }

  ClearAllScreen();
  SystemPoweroff();
}
