/*
 * bmp180.h
 *
 *  Created on: 27 feb. 2019
 *      Author: gheorghe.ghirjev
 */

#ifndef BMP180_H_
#define BMP180_H_


#include "stdint.h"


// Data type definitions:
#define BYTE_SHIFT					(8U)
#define SHORT_SHIFT					(16U)

/* Chip-id [R/O] (register D0h): This value is fixed to 0x55,
 * and can be used to check whether communication is functioning. */
#define BMP_CHIP_ID_REG			      		(0xD0)
#define BMP_CHIP_ID_VAL					(0x55)

/* Measurement control [R/W] (register F4h):
 * <4:0> - Controls measurements.
 * <5>   - SCO : Start of conversion. The value of this bit stays “1” during conversion,
 *               and is reset to “0” after conversion is complete
 * <7:6> - OSS : Controls the oversampling ratio of the pressure measurement
 */
#define BMP_CTRL_REG	         	       		(0xF4)
#define BMP_CTRL_SCO_BIT(reg)				((reg & 0x20) >> 5)
#define BMP_CTRL_OSS_MASK(oss)				(oss = (oss & 0x3) << 6)

#define BMP_OSS0_CONV_TIME				(5U)
#define BMP_OSS1_CONV_TIME				(8U)
#define BMP_OSS2_CONV_TIME				(14U)
#define BMP_OSS3_CONV_TIME				(26U)

/* Soft reset [W/O] (register E0h):
 * If set to 0xB6, will perform the same sequence as power on reset. */
#define BMP_SOFT_RST_REG 	                	(0xE0)
#define BMP_SOFT_RST_VAL  	                	(0xB6)

// Calibration data [R/O] (register AAh up to BFh):
#define BMP_CALIB_ADDR  	                	(0xAA)
#define BMP_CALIB_DATA_SIZE				(22U)

// Device I2C addr register [R/O]: write EEh, read EFh:
#define BMP_READ_ADDR	                    		(0xEF)
#define BMP_WRITE_ADDR		                	(0xEE)
#define BMP_I2C_TIMEOUT					(50U)
#define BMP_SET_I2CRW_REG(i2c_buff, reg)		(i2c_buff = reg)

// BMP measurmenet regs
#define BMP_DATA_MSB_ADDR	                	(0xF6)
#define BMP_DATA_LSB_ADDR	                	(0xF7)
#define BMP_DATA_XLSB_ADDR	                	(0xF8)

// Temp. measurement :
#define BMP_SET_TEMP_CONV		            	(0x2E)

#define BMP_TEMP_CONV_TIME				(5U)
#define BMP_MIN_TEMP_THRESHOLD				(-40)
#define BMP_MAX_TEMP_THRESHOLD				(85U)

// Pressure measurment :
#define BMP_SET_PRESS_CONV		            	(0x34)
#define BMP_PRESS_CONST_SEA_LEVEL 	    		(101325.0f) // pressure in Pa
#define BMP_PRESS_CONST_COEFICIENT 	    		(44330.0f)

// Altitude measurment :
#define BMP_MIN_ALT_THRESHOLD				(-500)	// m. relating to sea level)
#define BMP_MAX_ALT_THRESHOLD				(9000U)	// m. relating to sea level)



struct bmp_calib_param_t
{
    int16_t	 AC1;
    int16_t  AC2;
    int16_t  AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t  B1;
    int16_t  B2;
    int16_t  MB;
    int16_t  MC;
    int16_t  MD;
};

struct measure_data_t
{
    float temp;
    int32_t press;
    /* for uncompensated values do not use below fields: */
    int32_t B5;
    float altitude;
};

typedef enum
{
    ULTRA_LOW_PWR_MODE = 0,
    STANDARD_MODE,
    HIGH,
    ULTRA_HIGH_RESOLUTION
} oss_ratio_t;

typedef struct
{
    oss_ratio_t ratio;
    uint8_t wait_time;
}oss_t;

typedef enum
{
    NO_ERR = 0,
    CHIP_ID_INVALID_ERR,
    GET_CALIB_ERR,
    GET_TEMP_ERR,
    GET_PRESSURE_ERR,
    GET_ALTITUDE_ERR
} bmp_err_t;

typedef struct
{
    struct bmp_calib_param_t calib;
    struct measure_data_t uncomp;	// uncompensated values
    struct measure_data_t data;
    oss_t oss;
    bmp_err_t err;
}bmp_t;



/*!
* @brief:    - Performe initial sequence of BMP180 sensor.
* @param[in] - pointer to struct of type bmp_calib_param_t
* @return    - None.
*/
void bmp_init (bmp_t * bmp);

/*!
* @brief:    - Read oversampling settings from CTRL reg F4h.
* 			   Oss (register F4h <7:6>): controls the oversampling ratio of the pressure measurement
* 			   (00b: single, 01b: 2 times, 10b: 4 times, 11b: 8 times).
* @param[in] - None.
* @return    - None.
*/
static void set_oss (oss_t * oss, oss_ratio_t ratio);

/*!
* @brief:    - Get uncompensated temperature value.
* @param[in] - None.
* @return    - uncompensated temp.
*/
int32_t get_ut (void);

/*!
* @brief:    - Calc true temperature.
* @param[in] - pointer to struct of type bmp_t
* @return    - true temp.
*/
float get_temp(bmp_t * bmp);

/*!
* @brief:    - Get uncompensated pressure value.
* @param[in] - oss - conversion time.
* @return    - uncompensated pressure.
*/
int32_t get_up (oss_t oss);

/*!
* @brief:    - Calc true pressure.
* @param[in] - struct of type bmp_t
* @return    - true pressure in Pa.
*/
int32_t get_pressure(bmp_t bmp);

/*!
* @brief:    - Calc true altitude.
* @param[in] - struct of type bmp_t
* @return    - altitude in meters.
*/
float get_altitude (bmp_t * bmp);

/*
 *
 */
void bmp_disable(bmp_t * bmp);

#endif /* BMP180_H_ */
