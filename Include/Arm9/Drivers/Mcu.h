#pragma once
#include "Arm9/Common.h"

#define MCU_REG_FLAGS 0x0F
#define MCU_REG_BATTERY_PERCENT 0x0B
#define MCU_REG_BATTERY_FRACTION 0x0C
#define MCU_REG_SYSTEM_VOLTAGE 0x0D
#define MCU_REG_VOLUME_SLIDER 0x09
#define MCU_REG_3D_SLIDER 0x08
#define MCU_REG_POWER_CONTROL 0x20
#define MCU_REG_LCD_CONTROL 0x22
#define MCU_REG_POWER_LED_BRIGHTNESS 0x28
#define MCU_REG_POWER_MODE 0x29
#define MCU_REG_WIFI_LED_STATE 0x2A
#define MCU_REG_CAMERA_LED_STATE 0x2B
#define MCU_REG_3D_LED_STATE 0x2C
#define MCU_REG_RTC_TIME 0x36
#define MCU_REG_RTC_ALARM 0x38

typedef struct {
  uint8_t Seconds;
  uint8_t Minutes;
  uint8_t Hours;
  uint8_t Week;
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;
} McuRtcTime;

typedef struct {
  uint8_t Minutes;
  uint8_t Hours;
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;
} McuRtcAlarm;

void SystemReboot();
void SystemPoweroff();

uint8_t McuGetShellState();
bool McuIsChargerConnected();
bool McuIsBatteryCharging();
bool McuIsBottomScreenBacklightOn();
bool McuIsTopScreenBacklightOn();
bool McuIsLcdVoltageOn();
uint8_t McuGetBatteryPercentage();
float McuGetBatteryPercentagePrecise();
float McuGetVoltage();
uint8_t McuGetVolumeSlider();
uint8_t McuGet3DSlider();
uint8_t McuGetWifiLedState(void);
bool McuSetWifiLedState(uint8_t State);
uint8_t McuGetCameraLedState(void);
bool McuSetCameraLedState(uint8_t State);
uint8_t McuGet3DLedState(void);
bool McuSet3DLedState(uint8_t State);
uint8_t McuGetPowerLedBrightness(void);
bool McuSetPowerLedBrightness(uint8_t Brightness);
uint8_t McuGetPowerMode(void);
bool McuSetPowerMode(uint8_t Mode);
bool McuGetRtcTime(McuRtcTime *Time);
bool McuSetRtcTime(const McuRtcTime *Time);
bool McuGetRtcAlarm(McuRtcAlarm *Alarm);
bool McuSetRtcAlarm(const McuRtcAlarm *Alarm);
bool McuSetLcdControl(uint8_t Value);
bool McuSetPowerControl(uint8_t Value);
bool McuWriteRegister(uint8_t Register, uint8_t Value);
