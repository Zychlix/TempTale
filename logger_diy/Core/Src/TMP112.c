//
// Created by michal on 03.02.24.
//

#include "TMP112.h"


uint8_t TMP_Init(TMP112_t * instance)
{
    return 0;
}


uint8_t TMP_Read(TMP112_t * instance)
{
    uint8_t buffer;
    buffer = 0;

    if(HAL_I2C_Master_Transmit(instance->i2c,TMP_ADDRESS,&buffer,sizeof (buffer),100)!=HAL_OK)
    {
        return -1;
    }

    uint8_t receive_buffer[2];

    if(HAL_I2C_Master_Receive(instance->i2c,TMP_ADDRESS,&receive_buffer,sizeof (receive_buffer),100)!=HAL_OK)
    {
        return -1;
    }

    if(receive_buffer[0]&1<<7)
    {
        instance->temperature = (-(1<<16)+(receive_buffer[0]<<8 | receive_buffer[1])>>5);
    } else
    {
        instance->temperature = ((receive_buffer[0]<<8 | receive_buffer[1])>>5); //In tenths of a degree
    }
    return 0;
}