#include "ui.h"
#include "i2c.h"
#include "cache.h"
#include "qff.h"
#include "utils.h"
#include "console.h"

uint8_t *top_screen, *bottom_screen;

void main(int argc, char** argv) {
  if (argc >= 2) {
    uint8_t **fb = (uint8_t **)(void *)argv[1];
    top_screen = fb[0];
    bottom_screen = fb[2];
  } else {
    top_screen = (uint8_t *)(*(uint32_t *)0x23FFFE00);
    bottom_screen = (uint8_t *)(*(uint32_t *)0x23FFFE08);
  }

  console_t console = {top_screen, {}, 0};

  ClearScreenF(true, true, COLOR_STD_BG);
  
  console_puts(&console, "lunix v0.1.0\n");
  while (true) {
    console_draw(&console);
    if (!(system_shellstate() & 2)) {
      break;
    }
  }

  fs_deinit();
  ClearScreenF(true, true, COLOR_STD_BG);
  
  system_poweroff();
}
