#pragma once
#include "stm32l1xx_hal.h"

#define TMP_ADDRESS (0b1001000<<1)

typedef struct
{
    I2C_HandleTypeDef * i2c;
    int16_t temperature;
}TMP112_t;

uint8_t TMP_Init(TMP112_t * instance);

uint8_t TMP_Read(TMP112_t * instance);