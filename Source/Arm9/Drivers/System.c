#include "cache.h"
#include "i2c.h"
#include "timer.h"

void SystemReboot() {
  I2C_writeReg(I2C_DEV_MCU, 0x22, 1 << 0);
  flushEntireDCache();
  I2C_writeReg(I2C_DEV_MCU, 0x20, 1 << 2);
  while (true) {
  };
}

void SystemPoweroff() {
  I2C_writeReg(I2C_DEV_MCU, 0x22, 1 << 0);
  flushEntireDCache();
  I2C_writeReg(I2C_DEV_MCU, 0x20, 1 << 0);
  while (true) {
  };
}

uint8_t GetShellState() { return I2C_readReg(I2C_DEV_MCU, 0xF); }
