#pragma once

#include "stm32l1xx_hal.h"
/* This library can be used to drive TempTale Ultra built-in LCD panel using STM32L152 capabilities.
 *
 */


typedef enum
{
    TT_COM_0 = 0,
    TT_COM_1 = 2,
    TT_COM_2 = 4,
    TT_COM_3 = 6,
}TT_COM;


typedef enum
{
    TT_DIGIT_A = 0,
    TT_DIGIT_B = 1,
    TT_DIGIT_C = 2,

}TT_Digit_t;

typedef enum
{
    TT_SEGMENT_A=0,
    TT_SEGMENT_B=1,
    TT_SEGMENT_C=2,
    TT_SEGMENT_D=3,
    TT_SEGMENT_E=4,
    TT_SEGMENT_F=5,
    TT_SEGMENT_G=6,

}TT_Segment_t;

#define TT_SEG_A (1<<TT_SEGMENT_A)
#define TT_SEG_B (1<<TT_SEGMENT_B)
#define TT_SEG_C (1<<TT_SEGMENT_C)
#define TT_SEG_D (1<<TT_SEGMENT_D)
#define TT_SEG_E (1<<TT_SEGMENT_E)
#define TT_SEG_F (1<<TT_SEGMENT_F)
#define TT_SEG_G (1<<TT_SEGMENT_G)




typedef struct TT_Display
{
    LCD_HandleTypeDef * hlcd;

}TT_Display_t;

void TT_Write_COM(TT_Display_t * instance,TT_COM com, uint32_t mask, uint32_t value);
void TT_Clear(TT_Display_t * instance);
void TT_Segment_On(TT_Display_t * instance, TT_Digit_t digit, TT_Segment_t segment);
static TT_COM TT_Row_To_Com(uint8_t row);

void TT_Print_Digit(TT_Display_t * instance, TT_Digit_t digit, uint8_t number);