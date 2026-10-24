#include "Common/Cache.h"
#include "Drivers/I2c.h"

void SystemReboot() {
  I2cWriteRegister(I2C_DEV_MCU, 0x22, 1 << 0);
  flushEntireDCache();
  I2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 2);
  while (true) {
  };
}

void SystemPoweroff() {
  I2cWriteRegister(I2C_DEV_MCU, 0x22, 1 << 0);
  flushEntireDCache();
  I2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 0);
  while (true) {
  };
}

uint8_t GetShellState() { return I2cReadRegister(I2C_DEV_MCU, 0xF); }
