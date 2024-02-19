#pragma once
#include "TempTale_LCD.h"
#include "TMP112.h"
#include "BMP180.h"


#define TT_PRESSURE_RECORD_COUNT 50
typedef enum
{
    TT_Standby = 0,
    TT_Temperature = 1,
    TT_Altitude = 2,
    TT_Pressure = 3,
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
    uint8_t enter_standby;

    float pressure_vector[TT_PRESSURE_RECORD_COUNT];
    uint32_t current_pressure_record;
    uint8_t zero_set;

}TempTale_t;

int TempTale_init(TempTale_t * instance);

int TempTale_Temperature_Refresh(TempTale_t * instance);

int TempTale_Altitude_Refresh(TempTale_t * instance);

int TempTale_mode_executor(TempTale_t * instance);

int TT_start_button_click_handler();

int TT_stop_button_click_handler();

int TT_start_button_long_press_handler();

int TT_stop_button_long_press_handler();

int TT_toggle_mode(TempTale_t * instance);

int TT_add_pressure_record(TempTale_t * instance);

float TT_avg_altitude(TempTale_t * instance);