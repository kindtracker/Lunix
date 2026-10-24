#include "Drivers/I2c.h"

static const struct {
  uint8_t BusId;
  uint8_t RegisterAddr;
} DevData[] = {
    {0, 0x4A}, {0, 0x7A}, {0, 0x78}, {1, 0x4A}, {1, 0x78},
    {1, 0x2C}, {1, 0x2E}, {1, 0x40}, {1, 0x44}, {2, 0xD6},
    {2, 0xD0}, {2, 0xD2}, {2, 0xA4}, {2, 0x9A}, {2, 0xA0},
};

#define I2C_Device_Count (sizeof(DevData) / sizeof(DevData[0]))

uint8_t I2cGetDeviceBusId(uint8_t DeviceId) {
  return DeviceId < I2C_Device_Count ? DevData[DeviceId].BusId : 0xFF;
}

uint8_t I2cGetDeviceRegisterAddr(uint8_t DeviceId) {
  return DeviceId < I2C_Device_Count ? DevData[DeviceId].RegisterAddr : 0xFF;
}

static volatile uint8_t *DataRegisters[] = {
    (volatile uint8_t *)(I2c1_Register_OFF + I2C_Register_Data),
    (volatile uint8_t *)(I2c2_Register_OFF + I2C_Register_Data),
    (volatile uint8_t *)(I2c3_Register_OFF + I2C_Register_Data),
};

volatile uint8_t *I2cGetDataRegister(uint8_t BusId) {
  return BusId < 3 ? DataRegisters[BusId] : NULL;
}

static volatile uint8_t *CntRegisters[] = {
    (volatile uint8_t *)(I2c1_Register_OFF + I2C_Register_CNT),
    (volatile uint8_t *)(I2c2_Register_OFF + I2C_Register_CNT),
    (volatile uint8_t *)(I2c3_Register_OFF + I2C_Register_CNT),
};

volatile uint8_t *I2cGetCntRegister(uint8_t BusId) {
  return BusId < 3 ? CntRegisters[BusId] : NULL;
}

void I2cWaitBusy(uint8_t BusId) {
  volatile uint8_t *CntRegister = I2cGetCntRegister(BusId);

  if (CntRegister == NULL)
    return;

  while (*CntRegister & 0x80)
    ;
}

bool I2cGetResult(uint8_t BusId) {
  volatile uint8_t *CntRegister = I2cGetCntRegister(BusId);

  if (CntRegister == NULL)
    return false;

  I2cWaitBusy(BusId);
  return (*CntRegister >> 4) & 1;
}

uint8_t I2cGetData(uint8_t BusId) {
  volatile uint8_t *DataRegister = I2cGetDataRegister(BusId);
  return DataRegister != NULL ? *DataRegister : 0xFF;
}

void I2cStop(uint8_t BusId, uint8_t Arg0) {
  volatile uint8_t *CntRegister = I2cGetCntRegister(BusId);

  if (CntRegister == NULL)
    return;

  *CntRegister = (Arg0 << 5) | 0xC0;
  I2cWaitBusy(BusId);
  *CntRegister = 0xC5;
}

bool I2cSelectDevice(uint8_t BusId, uint8_t DevRegister) {
  volatile uint8_t *DataRegister = I2cGetDataRegister(BusId);
  volatile uint8_t *CntRegister = I2cGetCntRegister(BusId);

  if (DataRegister == NULL || CntRegister == NULL)
    return false;

  I2cWaitBusy(BusId);
  *DataRegister = DevRegister;
  *CntRegister = 0xC2;

  return I2cGetResult(BusId);
}

bool I2cSelectRegister(uint8_t BusId, uint8_t Register) {
  volatile uint8_t *DataRegister = I2cGetDataRegister(BusId);
  volatile uint8_t *CntRegister = I2cGetCntRegister(BusId);

  if (DataRegister == NULL || CntRegister == NULL)
    return false;

  I2cWaitBusy(BusId);
  *DataRegister = Register;
  *CntRegister = 0xC0;

  return I2cGetResult(BusId);
}

uint8_t I2cReadRegister(uint8_t DevId, uint8_t Register) {
  if (DevId >= I2C_Device_Count)
    return 0xFF;

  uint8_t BusId = I2cGetDeviceBusId(DevId);
  uint8_t DeviceRegisterAddr = I2cGetDeviceRegisterAddr(DevId);

  for (size_t Attempt = 0; Attempt < 8; Attempt++) {
    if (I2cSelectDevice(BusId, DeviceRegisterAddr) &&
        I2cSelectRegister(BusId, Register) &&
        I2cSelectDevice(BusId, DeviceRegisterAddr | 1)) {
      I2cWaitBusy(BusId);
      uint8_t Data = I2cGetData(BusId);
      I2cStop(BusId, 1);
      return Data;
    }

    *I2cGetCntRegister(BusId) = 0xC5;
    I2cWaitBusy(BusId);
  }

  return 0xFF;
}

bool I2cReadRegisterBuffer(unsigned int DevId, int Register, uint8_t *Buffer,
                           size_t BufferSize) {
  if (BufferSize == 0)
    return true;

  if (Buffer == NULL || Register < 0 || Register > 0xFF ||
      DevId >= I2C_Device_Count)
    return false;

  uint8_t BusId = I2cGetDeviceBusId((uint8_t)DevId);
  uint8_t DeviceRegisterAddr = I2cGetDeviceRegisterAddr((uint8_t)DevId);

  size_t Attempt = 0;

  while (!I2cSelectDevice(BusId, DeviceRegisterAddr) ||
         !I2cSelectRegister(BusId, (uint8_t)Register) ||
         !I2cSelectDevice(BusId, DeviceRegisterAddr | 1)) {
    *I2cGetCntRegister(BusId) = 0xC5;
    I2cWaitBusy(BusId);

    if (++Attempt >= 8)
      return false;
  }

  for (size_t Index = 0; Index < BufferSize; Index++) {
    I2cWaitBusy(BusId);

    if (Index == BufferSize - 1)
      *I2cGetCntRegister(BusId) = 0xE1;
    else
      *I2cGetCntRegister(BusId) = 0xF0;

    I2cWaitBusy(BusId);
    Buffer[Index] = I2cGetData(BusId);
  }

  return true;
}

bool I2cWriteRegister(uint8_t DevId, uint8_t Register, uint8_t Data) {
  if (DevId >= I2C_Device_Count)
    return false;

  uint8_t BusId = I2cGetDeviceBusId(DevId);
  uint8_t DeviceRegisterAddr = I2cGetDeviceRegisterAddr(DevId);

  for (size_t Attempt = 0; Attempt < 8; Attempt++) {
    if (I2cSelectDevice(BusId, DeviceRegisterAddr) &&
        I2cSelectRegister(BusId, Register)) {
      volatile uint8_t *DataRegister = I2cGetDataRegister(BusId);
      volatile uint8_t *CntRegister = I2cGetCntRegister(BusId);

      *DataRegister = Data;
      *CntRegister = 0xC1;
      I2cStop(BusId, 0);

      if (I2cGetResult(BusId))
        return true;
    }

    *I2cGetCntRegister(BusId) = 0xC5;
    I2cWaitBusy(BusId);
  }

  return false;
}
