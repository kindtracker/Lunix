#pragma once
#include "Arm9/Common.h"

#define I2c1_Register_OFF 0x10161000
#define I2c2_Register_OFF 0x10144000
#define I2c3_Register_OFF 0x10148000

#define I2C_Register_Data 0
#define I2C_Register_CNT 1
#define I2C_Register_CNTEX 2
#define I2C_Register_SCL 4

#define I2C_DEV_MCU 3
#define I2C_DEV_GYRO 10
#define I2C_DEV_IR 13

uint8_t I2cGetDeviceBusId(uint8_t DeviceId);
uint8_t I2cGetDeviceRegisterAddr(uint8_t DeviceId);

volatile uint8_t *I2cGetDataRegister(uint8_t BusId);
volatile uint8_t *I2cGetCntRegister(uint8_t BusId);

void I2cWaitBusy(uint8_t BusId);
bool I2cGetResult(uint8_t BusId);
uint8_t I2cGetData(uint8_t BusId);
void I2cStop(uint8_t BusId, uint8_t Arg0);

bool I2cSelectDevice(uint8_t BusId, uint8_t DevRegister);
bool I2cSelectRegister(uint8_t BusId, uint8_t Register);

uint8_t I2cReadRegister(uint8_t DevId, uint8_t Register);
bool I2cWriteRegister(uint8_t DevId, uint8_t Register, uint8_t Data);

bool I2cReadRegisterBuffer(unsigned int DevId, int Register, uint8_t *Buffer,
                           size_t BufferSize);
