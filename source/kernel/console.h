#pragma once

typedef struct {
  uint8_t *screen;
  char buffer[16384];
  int buffer_len;
} console_t;

void console_put(console_t *console, char chr);
void console_puts(console_t *console, const char *str);
void console_draw(console_t *console);
