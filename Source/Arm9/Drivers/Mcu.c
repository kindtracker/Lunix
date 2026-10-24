#include "Drivers/Mcu.h"
#include "Common/Cache.h"
#include "Drivers/I2c.h"

static uint8_t McuBcdToBinary(uint8_t Value) {
  return (uint8_t)(((Value >> 4) * 10) + (Value & 0x0F));
}

static uint8_t McuBinaryToBcd(uint8_t Value) {
  return (uint8_t)(((Value / 10) << 4) | (Value % 10));
}

void SystemReboot() {
  I2cWriteRegister(I2C_DEV_MCU, MCU_REG_LCD_CONTROL, 1 << 0);
  flushEntireDCache();
  I2cWriteRegister(I2C_DEV_MCU, MCU_REG_POWER_CONTROL, 1 << 2);

  while (true) {
  }
}

void SystemPoweroff() {
  I2cWriteRegister(I2C_DEV_MCU, MCU_REG_LCD_CONTROL, 1 << 0);
  flushEntireDCache();
  I2cWriteRegister(I2C_DEV_MCU, MCU_REG_POWER_CONTROL, 1 << 0);

  while (true) {
  }
}

uint8_t McuGetShellState() {
  return (I2cReadRegister(I2C_DEV_MCU, MCU_REG_FLAGS) >> 1) & 1;
}

bool McuIsChargerConnected() {
  return (I2cReadRegister(I2C_DEV_MCU, MCU_REG_FLAGS) >> 3) & 1;
}

bool McuIsBatteryCharging() {
  return (I2cReadRegister(I2C_DEV_MCU, MCU_REG_FLAGS) >> 4) & 1;
}

bool McuIsBottomScreenBacklightOn() {
  return (I2cReadRegister(I2C_DEV_MCU, MCU_REG_FLAGS) >> 5) & 1;
}

bool McuIsTopScreenBacklightOn() {
  return (I2cReadRegister(I2C_DEV_MCU, MCU_REG_FLAGS) >> 6) & 1;
}

bool McuIsLcdVoltageOn() {
  return (I2cReadRegister(I2C_DEV_MCU, MCU_REG_FLAGS) >> 7) & 1;
}

uint8_t McuGetBatteryPercentage() {
  return I2cReadRegister(I2C_DEV_MCU, MCU_REG_BATTERY_PERCENT);
}

float McuGetBatteryPercentagePrecise() {
  uint8_t Whole = I2cReadRegister(I2C_DEV_MCU, MCU_REG_BATTERY_PERCENT);
  uint8_t Fraction = I2cReadRegister(I2C_DEV_MCU, MCU_REG_BATTERY_FRACTION);

  return Whole + (Fraction / 256.0f);
}

float McuGetVoltage() {
  return I2cReadRegister(I2C_DEV_MCU, MCU_REG_SYSTEM_VOLTAGE) * (5.0f / 256.0f);
}

uint8_t McuGetVolumeSlider() {
  return I2cReadRegister(I2C_DEV_MCU, MCU_REG_VOLUME_SLIDER);
}

uint8_t McuGet3DSlider() {
  return I2cReadRegister(I2C_DEV_MCU, MCU_REG_3D_SLIDER);
}

uint8_t McuGetWifiLedState(void) {
  return I2cReadRegister(I2C_DEV_MCU, MCU_REG_WIFI_LED_STATE);
}

bool McuSetWifiLedState(uint8_t State) {
  return McuWriteRegister(MCU_REG_WIFI_LED_STATE, State & 0x0F);
}

uint8_t McuGetCameraLedState(void) {
  return I2cReadRegister(I2C_DEV_MCU, MCU_REG_CAMERA_LED_STATE);
}

bool McuSetCameraLedState(uint8_t State) {
  return McuWriteRegister(MCU_REG_CAMERA_LED_STATE, State & 0x0F);
}

uint8_t McuGet3DLedState(void) {
  return I2cReadRegister(I2C_DEV_MCU, MCU_REG_3D_LED_STATE);
}

bool McuSet3DLedState(uint8_t State) {
  return McuWriteRegister(MCU_REG_3D_LED_STATE, State & 0x0F);
}

uint8_t McuGetPowerLedBrightness(void) {
  return I2cReadRegister(I2C_DEV_MCU, MCU_REG_POWER_LED_BRIGHTNESS);
}

bool McuSetPowerLedBrightness(uint8_t Brightness) {
  return McuWriteRegister(MCU_REG_POWER_LED_BRIGHTNESS, Brightness);
}

uint8_t McuGetPowerMode(void) {
  return I2cReadRegister(I2C_DEV_MCU, MCU_REG_POWER_MODE);
}

bool McuSetPowerMode(uint8_t Mode) {
  return McuWriteRegister(MCU_REG_POWER_MODE, Mode);
}

bool McuGetRtcTime(McuRtcTime *Time) {
  if (Time == NULL) {
    return false;
  }

  Time->Seconds = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x36));

  Time->Minutes = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x36 + 1));

  Time->Hours = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x36 + 2));

  Time->Week = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x36 + 3));

  Time->Day = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x36 + 4));

  Time->Month = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x36 + 5));

  Time->Year = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x36 + 6));

  return true;
}

bool McuSetRtcTime(const McuRtcTime *Time) {
  bool Result = true;

  if (Time == NULL) {
    return false;
  }

  Result = McuWriteRegister(0x36, McuBinaryToBcd(Time->Seconds)) && Result;
  Result = McuWriteRegister(0x37 - 0, McuBinaryToBcd(Time->Minutes)) && Result;
  Result = McuWriteRegister(0x38, McuBinaryToBcd(Time->Hours)) && Result;
  Result = McuWriteRegister(0x39, McuBinaryToBcd(Time->Week)) && Result;
  Result = McuWriteRegister(0x3A, McuBinaryToBcd(Time->Day)) && Result;
  Result = McuWriteRegister(0x3B, McuBinaryToBcd(Time->Month)) && Result;
  Result = McuWriteRegister(0x3C, McuBinaryToBcd(Time->Year)) && Result;

  return Result;
}

bool McuGetRtcAlarm(McuRtcAlarm *Alarm) {
  if (Alarm == NULL) {
    return false;
  }

  Alarm->Minutes = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x38));
  Alarm->Hours = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x39));
  Alarm->Day = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x3A));
  Alarm->Month = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x3B));
  Alarm->Year = McuBcdToBinary(I2cReadRegister(I2C_DEV_MCU, 0x3C));

  return true;
}

bool McuSetRtcAlarm(const McuRtcAlarm *Alarm) {
  bool Result = true;

  if (Alarm == NULL) {
    return false;
  }

  Result = McuWriteRegister(0x38, McuBinaryToBcd(Alarm->Minutes)) && Result;
  Result = McuWriteRegister(0x39, McuBinaryToBcd(Alarm->Hours)) && Result;
  Result = McuWriteRegister(0x3A, McuBinaryToBcd(Alarm->Day)) && Result;
  Result = McuWriteRegister(0x3B, McuBinaryToBcd(Alarm->Month)) && Result;
  Result = McuWriteRegister(0x3C, McuBinaryToBcd(Alarm->Year)) && Result;

  return Result;
}

bool McuWriteRegister(uint8_t Register, uint8_t Value) {
  return I2cWriteRegister(I2C_DEV_MCU, Register, Value);
}

bool McuSetLcdControl(uint8_t Value) {
  return McuWriteRegister(MCU_REG_LCD_CONTROL, Value);
}

bool McuSetPowerControl(uint8_t Value) {
  return McuWriteRegister(MCU_REG_POWER_CONTROL, Value);
}
