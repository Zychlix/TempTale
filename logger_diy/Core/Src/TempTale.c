//
// Created by michal on 16.02.24.
//

#include "TempTale.h"

void Display_Temperature(TT_Display_t * instance,int16_t temperature)
{
    TT_Write_Segment(instance,LCD_CENTIGRADE_COL,0xffffffff,LCD_CENTIGRADE_PIN);
    TT_Write_Segment(instance,LCD_TEMP_COL,0xffffffff,LCD_TEMP_PIN);
    TT_Write_Segment(instance,LCD_CENTIGRADE_BAR_COL,0xffffffff,LCD_CENTIGRADE_BAR_PIN);
    TT_Display_Decimal(instance,temperature,TT_TENTHS); // Display in 10's

}

int TempTale_init(TempTale_t * instance)
{
    instance->state = TT_Pressure;
    instance->button_pressed=0;
    instance->zero_altitude = instance->pressure_sensor->data.altitude;
    return 0;
}

int TempTale_Temperature_Refresh(TempTale_t * instance)
{
    Display_Temperature(instance->lcd, instance->temperature_sensor->temperature);

    return 0;
}
int TempTale_Pressure_Refresh(TempTale_t * instance)
{
    TT_Display_Decimal(instance->lcd,(instance->pressure_sensor->data.altitude-instance->zero_altitude)*10,TT_TENTHS); // Display in 10's
    TT_Write_Segment(instance->lcd,TT_COM_1,0xffffffff,(1<<27));

    return 0;
}

int TempTale_mode_executor(TempTale_t * instance)
{
    switch (instance->state) {
        case TT_Temperature:
            TempTale_Temperature_Refresh(instance);
            break;
        case TT_Pressure:
            TempTale_Pressure_Refresh(instance);
            break;
        default:

    }
    return 0;
}

int TT_toggle_mode(TempTale_t * instance)
{
    switch (instance->state) {
        case TT_Pressure:
            instance->state = TT_Temperature;
            break;
        case TT_Temperature:
            instance->state = TT_Pressure;
//            HAL_PWR_EnableSleepOnExit();
//            __WFI();
            break;
        default:
    }
    return 0;
}

