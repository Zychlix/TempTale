#pragma once
#include "TempTale_LCD.h"
#include "TMP112.h"
#include "BMP180.h"

typedef enum
{
    TT_Standby = 0,
    TT_Temperature = 1,
    TT_Pressure = 2,
}TempTale_state_t;

typedef struct
{
    TT_Display_t * lcd;
    bmp_t * pressure_sensor;
    TMP112_t * temperature_sensor;
    TempTale_state_t state;

    float zero_altitude;

    uint32_t button_press_time;
    uint8_t button_pressed;

}TempTale_t;

int TempTale_init(TempTale_t * instance);

int TempTale_Temperature_Refresh(TempTale_t * instance);

int TempTale_Pressure_Refresh(TempTale_t * instance);

int TempTale_mode_executor(TempTale_t * instance);

int TT_start_button_click_handler();

int TT_stop_button_click_handler();

int TT_toggle_mode(TempTale_t * instance);