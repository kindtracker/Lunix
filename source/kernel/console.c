#include "ui.h"
#include "common.h"
#include "console.h"

// TODO: dont overflow buffer
void console_put(console_t *console, char chr) {
  console->buffer[console->buffer_len++] = chr;
}

void console_puts(console_t *console, const char *str) {
  for (int i = 0; i < strlen(str); i++) {
    console_put(console, str[i]);
  }
}

void console_draw(console_t *console) {
  int x = 0;
  int y = 0;
  for (int i = 0; i < console->buffer_len; i++) {
    char chr = console->buffer[i];
    if (chr == '\n') {
      x = 0;
      y += 8;
    }
    DrawCharacter(console->screen, chr, x, y, COLOR_STD_FONT, COLOR_STD_BG);
    x += 8;
  }
}
