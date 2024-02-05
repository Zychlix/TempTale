//
// Created by michal on 03.02.24.
//

#include "TempTale_LCD.h"

#define TT_DIG_A_AD_OFFSET 5
#define TT_DIG_A_EG_OFFSET 23

#define TT_DIG_B_AD_OFFSET 12
#define TT_DIG_B_EG_OFFSET 6

#define TT_DIG_C_AD_OFFSET 24
#define TT_DIG_C_EG_OFFSET 15

const uint8_t TT_BCD_LUT[] =
        {
                TT_SEG_A | TT_SEG_B | TT_SEG_C | TT_SEG_D | TT_SEG_E | TT_SEG_F,
                TT_SEG_B | TT_SEG_C,
                TT_SEG_A | TT_SEG_B | TT_SEG_G | TT_SEG_E | TT_SEG_D,
                TT_SEG_A | TT_SEG_B | TT_SEG_C | TT_SEG_D | TT_SEG_G,
                TT_SEG_F | TT_SEG_B | TT_SEG_G | TT_SEG_C,
                TT_SEG_A | TT_SEG_F | TT_SEG_G | TT_SEG_C | TT_SEG_D,
                TT_SEG_A | TT_SEG_F | TT_SEG_G | TT_SEG_C | TT_SEG_D | TT_SEG_E,
                TT_SEG_A | TT_SEG_B | TT_SEG_C,
                TT_SEG_A | TT_SEG_B | TT_SEG_C | TT_SEG_D | TT_SEG_E | TT_SEG_F | TT_SEG_G,
                TT_SEG_A | TT_SEG_F | TT_SEG_C | TT_SEG_D | TT_SEG_B | TT_SEG_G,

        };

void TT_Write_Segment(TT_Display_t * instance, TT_COM com, uint32_t mask, uint32_t value)
{
    HAL_LCD_Write(instance->hlcd, com, mask, value);
//    HAL_LCD_UpdateDisplayRequest(instance->hlcd);
}


void TT_Clear(TT_Display_t * instance)
{
    HAL_LCD_Clear(instance->hlcd);
}

void TT_Segment_On(TT_Display_t * instance, TT_Digit_t digit, TT_Segment_t segment)
{
    uint8_t lcd_seg_AD =0;
    uint8_t lcd_seg_EG =0;

    switch (digit) {
        case TT_DIGIT_A:
            lcd_seg_AD = TT_DIG_A_AD_OFFSET;
            lcd_seg_EG = TT_DIG_A_EG_OFFSET;
            break;
        case TT_DIGIT_B:
            lcd_seg_AD = TT_DIG_B_AD_OFFSET;
            lcd_seg_EG = TT_DIG_B_EG_OFFSET;
            break;
        case TT_DIGIT_C:
            lcd_seg_AD = TT_DIG_C_AD_OFFSET;
            lcd_seg_EG = TT_DIG_C_EG_OFFSET;
            break;
        default:
            break;
    }


    uint8_t row = 0;
    uint8_t pin = 0;
    if(segment <= TT_SEGMENT_D)
    {
        row = segment;
        pin = lcd_seg_AD;
    }
    if(segment > TT_SEGMENT_D && segment<=TT_SEGMENT_G)
    {
        row = segment-4;
        pin = lcd_seg_EG;
    }

    TT_Write_Segment(instance, TT_Row_To_Com(row), 0xFFFFFFFF, (1 << pin));

}

static TT_COM TT_Row_To_Com(uint8_t row)
{
    return row<<1;
}

void TT_Print_Digit(TT_Display_t * instance, TT_Digit_t digit, uint8_t number)
{
    if(number>9)
    {
        return;
    }
    for(int i =0; i<7; i++)
    {
        if(TT_BCD_LUT[number]&(1<<i))
        {
            TT_Segment_On(instance,digit, i);
        }
    }
    //HAL_LCD_UpdateDisplayRequest(instance->hlcd);

}

void TT_Display_Integer(TT_Display_t * instance, int16_t number)
{
    uint16_t accumulator;
    if(number<0)
    {
        TT_Write_Segment(instance, LCD_MINUS_COL, 0xFFFFFFFF, LCD_MINUS_PIN);
        accumulator = -number;
    } else
    {
        accumulator = number;
    }


    uint8_t zero_inhibit = 1;    //Trailing zero


    if(accumulator>= 1000)       //Get rid of one, treat as 3 digit
    {
        zero_inhibit = 0;
        TT_Write_Segment(instance, LCD_ONE_COL, 0xFFFFFFFF, LCD_ONE_PIN);
        accumulator-=1000;

    }

    uint16_t  position_value;

    position_value = accumulator/100;

    accumulator = accumulator- position_value*100;
    if(position_value)
    {
        zero_inhibit = 0;
        TT_Print_Digit(instance,TT_DIGIT_C,position_value);
    }else
    {
        if(!zero_inhibit)
        {
            TT_Print_Digit(instance, TT_DIGIT_C, position_value);
        }
    }


    position_value = accumulator/10;
    accumulator = accumulator- position_value*10;

    if(position_value)
    {
        TT_Print_Digit(instance, TT_DIGIT_B, position_value);
        zero_inhibit = 0;
    } else
    {
        if(!zero_inhibit)
        {
            TT_Print_Digit(instance, TT_DIGIT_B, position_value);
        }
    }

    TT_Print_Digit(instance, TT_DIGIT_A, accumulator);
}

void TT_Display_Decimal(TT_Display_t * instance, int16_t number, TT_Decimals_t point)
{
    TT_Display_Integer(instance,number);

    switch (point) {
        case TT_ONES:
            break;
        case TT_TENTHS:
            TT_Write_Segment(instance,LCD_TENTH_COL,0xFFFFFFFF,LCD_TENTH_PIN);

    }
}