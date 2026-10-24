#include "i2c.h"
#include "cache.h"
#include "timer.h"

#define TICKS_PER_SEC       67027964ULL
#define REG_TIMER_CNT(i)    *(volatile uint16_t *)(0x10003002 + 4 * i)
#define REG_TIMER_VAL(i)    *(volatile uint16_t *)(0x10003000 + 4 * i)

#define MAKE_BRANCH(src, dst)      (0xEA000000 | ((uint32_t)((((uint8_t *)(dst) - (uint8_t *)(src)) >> 2) - 2) & 0xFFFFFF))
#define MAKE_BRANCH_LINK(src, dst) (0xEB000000 | ((uint32_t)((((uint8_t *)(dst) - (uint8_t *)(src)) >> 2) - 2) & 0xFFFFFF))

void system_reboot() {
  I2C_writeReg(I2C_DEV_MCU, 0x22, 1 << 0);
  flushEntireDCache();
  I2C_writeReg(I2C_DEV_MCU, 0x20, 1 << 2);
  while (true) {};
}

void system_poweroff() {
  I2C_writeReg(I2C_DEV_MCU, 0x22, 1 << 0);
  flushEntireDCache();
  I2C_writeReg(I2C_DEV_MCU, 0x20, 1 << 0);
  while (true) {};
}

uint8_t system_shellstate() {
  return I2C_readReg(I2C_DEV_MCU, 0xF);
}

void system_wait(u64 ms) {
  timer_start();
  u64 initial_value = timer_msec();
  while (timer_msec() - initial_value < ms);
}
