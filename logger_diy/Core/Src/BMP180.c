/*
 * bmp180.c
 *
 *  Created on: 27 feb. 2019
 *      Author: gheorghe.ghirjev
 */

#include <string.h>
#include <math.h>
#include "BMP180.h"
#include "stm32l1xx_hal.h"


// global struct to store all BMP180 main data.
bmp_t bmp;

extern I2C_HandleTypeDef hi2c1;



/*!
* @brief:    - Read and check bmp chip ID.This value is fixed to 0x55,
*              and can be used to check whether communication is functioning.
* @param[in] - NONE.
* @return    - NO_ERR if chip_id is equal to 0x55, otherwise CHIP_ID_INVALID_ERR.
*/
static bmp_err_t read_chip_id (void)
{
    uint8_t out_buff = 0;
    uint8_t ret_val = NO_ERR;

    HAL_I2C_Mem_Read(&hi2c1, BMP_READ_ADDR, BMP_CHIP_ID_REG, 1, &out_buff, 1, BMP_I2C_TIMEOUT);

    if (BMP_CHIP_ID_VAL != out_buff)
    {
        ret_val = CHIP_ID_INVALID_ERR;
    }

    return ret_val;
}


/*!
* @brief:    - Write oversampling settings to the Control register.
* @param[in] - struct of type oss_t
* @param[in] - enum of type oss_ratio_t
* @return    - None
*/
static void set_oss (oss_t * oss, oss_ratio_t ratio)
{
    uint8_t in_buff[2] = {0};

    switch (ratio)
    {
        case ULTRA_LOW_PWR_MODE:
        {
            oss->wait_time = BMP_OSS0_CONV_TIME;
            break;
        }
        case STANDARD_MODE:
        {
            oss->wait_time = BMP_OSS1_CONV_TIME;
            break;
        }
        case HIGH:
        {
            oss->wait_time = BMP_OSS2_CONV_TIME;
            break;
        }
        case ULTRA_HIGH_RESOLUTION:
        {
            oss->wait_time = BMP_OSS3_CONV_TIME;
            break;
        }
        default:
        {
            oss->wait_time = BMP_OSS1_CONV_TIME;
            break;
        }
    }

    oss->ratio = ratio;
    BMP_SET_I2CRW_REG (in_buff[1], BMP_CTRL_OSS_MASK(ratio));
    HAL_I2C_Mem_Write( &hi2c1, BMP_WRITE_ADDR, BMP_CTRL_REG, 1, in_buff, 2, BMP_I2C_TIMEOUT );
}


/*!
* @brief:    - Read calibration BMP data. The E2PROM has stored 176 bit of individual calibration data.
*              This is used to compensate offset, temperature dependence and other parameters of the sensor.
* @param[in] - struct of type bmp_calib_param_t
* @return    - NO_ERR if read calibration data are valid otherwise READ_CALIB_ERR.
*/
static bmp_err_t read_calib_data (short * calib_data)
{
    bmp_err_t ret_val = NO_ERR;
    uint8_t out_buff[BMP_CALIB_DATA_SIZE] = {0};
    uint8_t i = 0;
    uint8_t j = 1;

    HAL_I2C_Mem_Read(&hi2c1, BMP_READ_ADDR, BMP_CALIB_ADDR, 1, out_buff, BMP_CALIB_DATA_SIZE, BMP_I2C_TIMEOUT);

    // Store read calib data to bmp_calib struct.
    for (i = 0; i <= BMP_CALIB_DATA_SIZE / 2; i++, j+2)
    {
        calib_data[i] = (out_buff[i * 2] << 8) | out_buff[j];

        // checking that none of the words has the value 0 or 0xFFFF.
        if ((0 == calib_data[i]) | (-1 == calib_data[i]))
        {
            ret_val = GET_CALIB_ERR;
        }
    }

    return ret_val;
}

/*!
* @brief:    - Performe initial sequence of BMP sensor
* @param[in] - pointer to struct of type bmp_calib_param_t
* @return    - None.
*/
void bmp_init (bmp_t * bmp)
{
    memset(bmp, 0x00, sizeof(&bmp)); // clear bmp strut;
    bmp->err = read_chip_id ();      // check chip validity and I2C communication.
    bmp->err = read_calib_data ((short *)&bmp->calib);
    set_oss (&bmp->oss, ULTRA_HIGH_RESOLUTION);       // set oversampling settings
}

/*!
* @brief:    - Get uncompensated temperature value. UT = temperature data (16 bit)
* @param[in] - None.
* @return    - uncompensated temp.
*/
int32_t get_ut (void)
{
    uint8_t out_buff[2];

    BMP_SET_I2CRW_REG (out_buff[0], BMP_SET_TEMP_CONV);
    HAL_I2C_Mem_Write( &hi2c1, BMP_WRITE_ADDR, BMP_CTRL_REG, 1, out_buff, 1, BMP_I2C_TIMEOUT );
    HAL_Delay (BMP_TEMP_CONV_TIME);
    HAL_I2C_Mem_Read ( &hi2c1, BMP_READ_ADDR, BMP_DATA_MSB_ADDR, 1, out_buff, 2, BMP_I2C_TIMEOUT );

    return (out_buff[0] << BYTE_SHIFT) | out_buff[1];
}

/*!
* @brief:    - Calc true temperature.
* @param[in] - pointer to struct of type bmp_t
* @return    - true temp.
*/
float get_temp(bmp_t * bmp)
{
    int32_t X1 = 0;
    int32_t X2 = 0;
    float temp = 0;

    X1 = (((int32_t)bmp->uncomp.temp - bmp->calib.AC6) * bmp->calib.AC5) >> 15;
    X2 = (bmp->calib.MC << 11) / (X1 + bmp->calib.MD);
    bmp->data.B5 = X1 + X2;
    temp = ((bmp->data.B5 + 8) >> 4) * 0.1f;

    if ((temp <= BMP_MIN_TEMP_THRESHOLD) || (temp >= BMP_MAX_TEMP_THRESHOLD))
    {
        bmp->err = GET_TEMP_ERR;
    }

    return temp;
}

/*!
* @brief:    - Get uncompensated pressure value. UP = pressure data (16 to 19 bit)
* @param[in] - struct of type oss_t
* @return    - uncompensated pressure.
*/
int32_t get_up (oss_t oss)
{
    uint8_t out_buff[3] = {0};
    long up = 0;

    BMP_SET_I2CRW_REG (out_buff[0], BMP_SET_PRESS_CONV);
    HAL_I2C_Mem_Write ( &hi2c1, BMP_WRITE_ADDR, BMP_CTRL_REG, 1, out_buff, 1, BMP_I2C_TIMEOUT );
    HAL_Delay (oss.wait_time);
    HAL_I2C_Mem_Read(&hi2c1, BMP_READ_ADDR, BMP_DATA_MSB_ADDR, 1, out_buff, 3, BMP_I2C_TIMEOUT);

    up = ((out_buff[0] << SHORT_SHIFT) + (out_buff[1] << BYTE_SHIFT) + out_buff[2]) >> (8 - oss.ratio);
    return up;
}

/*!
* @brief:    - Calc true pressure.
* @param[in] - struct of type bmp_t
* @return    - true pressure in Pa.
*/
int32_t get_pressure(bmp_t bmp)
{
    int32_t X1, X2, X3, B3, B6, p = 0;
    uint32_t B4, B7 = 0;

    B6 = bmp.data.B5 - 4000;
    X1 = (bmp.calib.B2 * (B6 * B6 / 0x1000)) / 0x800;
    X2 = bmp.calib.AC2 * B6 / 0x800;
    X3 = X1 + X2;
    B3 = (((bmp.calib.AC1 * 4 + X3) << bmp.oss.ratio) +2) / 4;
    X1 = bmp.calib.AC3 * B6 / 0x2000;
    X2 = (bmp.calib.B1 * (B6 * B6 / 0x1000)) / 0x10000;
    X3 = ((X1 + X2) + 2) / 0x4;
    B4 = bmp.calib.AC4 * (unsigned long)(X3 + 32768) / 0x8000;
    B7 = ((unsigned long)bmp.uncomp.press - B3) * (50000 >> bmp.oss.ratio);

    if (B7 < 0x80000000)
    {
        p = (B7 * 2) / B4;
    }
    else
    {
        p = (B7 / B4) * 2;
    }

    X1 = (p / 0x100 * (p / 0x100));
    X1 = (X1 * 3038) / 0x10000;
    X2 = (-7357 * p) / 0x10000;
    p = p + (X1 + X2 + 3791) / 0x10;

    return p;
}

/*!
* @brief:    - Calc true altitude.
* @param[in] - struct of type bmp_t
* @return    - true pressure.
*/
float get_altitude (bmp_t * bmp)
{
    float altitude = 0;

    altitude = BMP_PRESS_CONST_COEFICIENT * (1.0f - pow((bmp->data.press / BMP_PRESS_CONST_SEA_LEVEL), (1/5.255)));

    if ((altitude <= BMP_MIN_ALT_THRESHOLD) || (altitude >= BMP_MAX_ALT_THRESHOLD))
    {
        bmp->err = GET_ALTITUDE_ERR;
    }

    return altitude;
}



