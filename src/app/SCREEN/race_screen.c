#include "race_screen.h"
#include "display.h"
#include "DataManager.h"
#include "RA8875.h"
#include "rtt.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32g4xx_hal.h"


#define BAR_SPACING_X 5
#define BAR_SPACING_Y 5


#define STATUS_X          0
#define STATUS_Y          0
#define STATUS_W          SCREEN_WIDTH
#define STATUS_H          50
#define STATUS_TEXT_SCALE 2

#define NOT_READY_STATE_COLOR         RA8875_RED
#define INVERTERS_POWERED_STATE_COLOR RA8875_CYAN
#define PRECHARGING_STATE_COLOR       RA8875_ORANGE
#define WAIT_STATE_COLOR              RA8875_YELLOW
#define STANDBY_STATE_COLOR           RA8875_WHITE
#define READY_TO_DRIVE_STATE_COLOR    RA8875_GREEN
#define SHUTDOWN_STATE_COLOR          RA8875_RED

#define NUM_BARS 5
#define BAR_WIDTH (SCREEN_WIDTH - ((NUM_BARS - 1) * BAR_SPACING_X)) / NUM_BARS
#define BAR_HEIGHT (SCREEN_HEIGHT - (STATUS_H + BAR_SPACING_Y))

#define BAR_VALUE_HEIGHT 20
#define BAR_TITLE_HEIGHT 20
#define BAR_Y (STATUS_H + BAR_SPACING_Y)
#define BAR_X_OFFSET (BAR_WIDTH + BAR_SPACING_X)

#define MIN_CELL_VOLT_LOW   300  // 3.00 V
#define MIN_CELL_VOLT_HIGH  430  // 4.30 V
                        
#define MAX_CELL_TEMP_LOW   150  // 15 C
#define MAX_CELL_TEMP_HIGH  600  // 60 C

#define PACK_V_LOW          MIN_CELL_VOLT_LOW  * 135 / 10
#define PACK_V_HIGH         MIN_CELL_VOLT_HIGH * 135 / 10

#define MOTOR_TEMP_LOW      150   //  15 C
#define MOTOR_TEMP_HIGH     1500  // 150 C

#define INV_TEMP_LOW        150   //  15 C
#define INV_TEMP_HIGH       600   //  60 C
                                
 #define NUM_BAR_COLORS 3

enum
{
    MIN_CELL_COLUMN = 0,
    MAX_TEMP_COLUMN,
    PACK_V_COLUMN,
    MOTOR_TEMP_COLUMN,
    INVERTER_TEMP_COLUMN
};
/*
static const uint16_t BAR_COLORS[] =
{
    0x07E0, // GREEN
    0xFFE0, // YELLOW
    0xF800, // RED
};

 
static const uint16_t BAR_COLORS[] =
{
    0x07E0, // Green
    0x5FE0, // Yellow-Green
    0xFFE0, // Yellow
    0xFD20, // Orange
    0xF800  // Red
};
*/
static const uint16_t BAR_COLORS[] =
{
    0x07E0, // Green
    0x3FE0, // Light Green
    0x7FE0, // Lime
    0xFFE0, // Yellow
    0xFEC0, // Yellow-Orange
    0xFD20, // Orange
    0xFA20, // Dark Orange
    0xF800  // Red
};


static uint16_t calculate_percentage(uint16_t value, uint16_t low, uint16_t high)
{
    if (high <= low)   return 0;
    if (value <= low)  return 0;
    if (value >= high) return 100;
    
    return ((value - low) * 100) / (high - low);
}

static uint16_t voltage_bar_color(uint16_t percentage)
{
    if (percentage > 100) percentage = 100;

    uint8_t index = ((100 - percentage) * (NUM_BAR_COLORS - 1)) / 100;

    return BAR_COLORS[index];
}

static uint16_t temperature_bar_color(uint16_t percentage)
{
    if (percentage > 100) percentage = 100;

    uint8_t index = (percentage * (NUM_BAR_COLORS - 1)) / 100;

    return BAR_COLORS[index];
}

/*
static bool value_irrational(uint16_t value, uint16_t low, uint16_t high)
{
    return (value < low || value > high);
}
*/

static display_bar_t min_cell_bar =
{
    .title = "MIN CELL",
    .title_h = BAR_TITLE_HEIGHT,
    .value_h = BAR_VALUE_HEIGHT,
    .x = BAR_X_OFFSET * MIN_CELL_COLUMN,
    .y = BAR_Y,
    .w = BAR_WIDTH,
    .h = BAR_HEIGHT
};

static display_bar_t max_temp_bar = 
{
    .title = "MAX TEMP",
    .title_h = BAR_TITLE_HEIGHT,
    .value_h = BAR_VALUE_HEIGHT,
    .x = BAR_X_OFFSET * MAX_TEMP_COLUMN,
    .y = BAR_Y,
    .w = BAR_WIDTH,
    .h = BAR_HEIGHT
};

static display_bar_t pack_v_bar = 
{
    .title = "PACK V",
    .title_h = BAR_TITLE_HEIGHT,
    .value_h = BAR_VALUE_HEIGHT,
    .x = BAR_X_OFFSET * PACK_V_COLUMN,
    .y = BAR_Y,
    .w = BAR_WIDTH,
    .h = BAR_HEIGHT
};

static display_bar_t motor_temp_bar = 
{
    .title = "MOTOR TEMP",
    .title_h = BAR_TITLE_HEIGHT,
    .value_h = BAR_VALUE_HEIGHT,
    .x = BAR_X_OFFSET * MOTOR_TEMP_COLUMN,
    .y = BAR_Y,
    .w = BAR_WIDTH,
    .h = BAR_HEIGHT
};

static display_bar_t inv_temp_bar = 
{
    .title = "INV TEMP",
    .title_h = BAR_TITLE_HEIGHT,
    .value_h = BAR_VALUE_HEIGHT,
    .x = BAR_X_OFFSET * INVERTER_TEMP_COLUMN,
    .y = BAR_Y,
    .w = BAR_WIDTH,
    .h = BAR_HEIGHT
};

void draw_race_screen(void)
{
    RA8875_graphic_mode();
    RA8875_fill_screen(RA8875_BLACK);

//--------------------STATUS--------------------//    
    
    display_status_banner(
        STATUS_X,                  
        STATUS_Y,                  
        STATUS_W,              
        STATUS_H,                  
        STATUS_TEXT_SCALE,                                
        "NOT READY",
        RA8875_RED,
        RA8875_BLACK
    );

//-----------------MIN CELL BAR-----------------// 

    display_draw_bar(&min_cell_bar,
                     "?.?? V",
                     RA8875_WHITE, 
                     RA8875_BLACK
    );

//-----------------MAX TEMP BAR-----------------// 

    display_draw_bar(&max_temp_bar,
                     "??.? C",
                     RA8875_WHITE, 
                     RA8875_BLACK
    );

//------------------PACK V BAR------------------// 

    display_draw_bar(&pack_v_bar,
                     "??? V",
                     RA8875_WHITE, 
                     RA8875_BLACK
    );

//----------------MOTOR TEMP BAR----------------// 

    display_draw_bar(&motor_temp_bar, 
                     "???.? C",
                     RA8875_WHITE, 
                     RA8875_BLACK
    );

//-----------------INV TEMP BAR-----------------// 
    
    display_draw_bar(&inv_temp_bar,
                     "??.? C",
                     RA8875_WHITE, 
                     RA8875_BLACK
    );
}


void update_race_screen(void)
{
    DataManager_update_race_data();
    
    if (race_dash_data.min_cell_flag)
    {
        update_race_min_cell_bar();
        race_dash_data.min_cell_flag = false;
    }

    if (race_dash_data.max_temp_flag)
    {
        update_race_max_temp_bar();
        race_dash_data.max_temp_flag = false;
    }

    if (race_dash_data.pack_v_flag)
    {
        update_race_pack_v_bar();
        race_dash_data.pack_v_flag = false;
    }

    if (race_dash_data.motor_temp_flag)
    {
        update_race_motor_temp_bar();
        race_dash_data.motor_temp_flag = false;
    }

    if (race_dash_data.inv_temp_flag)
    {
        update_race_inv_temp_bar();
        race_dash_data.inv_temp_flag = false;
    }

    if (race_dash_data.vc_status_flag)
    { 
        update_race_status_bar();
        race_dash_data.vc_status_flag = false;
    }
}

void update_race_status_bar()
{    
    uint8_t state = race_dash_data.vc_status.vc_status_vehicle_state;

    switch (state)
    { 
        case NOT_READY_STATE :
            display_status_banner
            (
                STATUS_X,
                STATUS_Y,
                STATUS_W,
                STATUS_H,
                STATUS_TEXT_SCALE,
                VC_STATUS_MESSAGES[state],
                NOT_READY_STATE_COLOR,
                RA8875_BLACK
            );  break;

        case INVERTERS_POWERED_STATE :
            display_status_banner
            (
                STATUS_X,
                STATUS_Y,
                STATUS_W,
                STATUS_H,
                STATUS_TEXT_SCALE,
                VC_STATUS_MESSAGES[state],
                INVERTERS_POWERED_STATE_COLOR,
                RA8875_BLACK
            );  break;

        case PRECHARGING_STATE :
            display_status_banner
            (
                STATUS_X,
                STATUS_Y,
                STATUS_W,
                STATUS_H,
                STATUS_TEXT_SCALE,
                VC_STATUS_MESSAGES[state],
                PRECHARGING_STATE_COLOR, 
                RA8875_BLACK
            );  break;

        case WAIT_STATE :
            display_status_banner
            (
                STATUS_X,
                STATUS_Y,
                STATUS_W,
                STATUS_H,
                STATUS_TEXT_SCALE,
                VC_STATUS_MESSAGES[state],
                WAIT_STATE_COLOR,
                RA8875_BLACK
            );  break;

        case STANDBY_STATE :
            display_status_banner
            (
                STATUS_X,
                STATUS_Y,
                STATUS_W,
                STATUS_H,
                STATUS_TEXT_SCALE,
                VC_STATUS_MESSAGES[state],
                STANDBY_STATE_COLOR,
                RA8875_BLACK
            );  break;

        case READY_TO_DRIVE_STATE :
            display_status_banner
            (
                STATUS_X,
                STATUS_Y,
                STATUS_W,
                STATUS_H,
                STATUS_TEXT_SCALE,
                VC_STATUS_MESSAGES[state],
                READY_TO_DRIVE_STATE_COLOR,
                RA8875_BLACK
            );  break;

        case SHUTDOWN_STATE :
            display_status_banner
            (
                STATUS_X,
                STATUS_Y,
                STATUS_W,
                STATUS_H,
                STATUS_TEXT_SCALE,
                VC_STATUS_MESSAGES[state],
                SHUTDOWN_STATE_COLOR,
                RA8875_BLACK
            );  break;

        default: return;
    }
}

void update_race_min_cell_bar()
{             
    char buf[10];
    snprintf(buf, sizeof(buf), "%u.%02u V", race_dash_data.hvbms_min_cell / 100, race_dash_data.hvbms_min_cell % 100);

    uint16_t bar_percentage = calculate_percentage(race_dash_data.hvbms_min_cell, MIN_CELL_VOLT_LOW, MIN_CELL_VOLT_HIGH);
    uint16_t bar_color = voltage_bar_color(bar_percentage);

    display_update_bar(&min_cell_bar, buf, bar_percentage, bar_color);
}

void update_race_max_temp_bar()
{
    char buf[10];
    snprintf(buf, sizeof(buf), "%u.%01u C", race_dash_data.hvbms_max_temp / 10, race_dash_data.hvbms_max_temp % 10);

    uint16_t bar_percentage = calculate_percentage(race_dash_data.hvbms_max_temp, MAX_CELL_TEMP_LOW, MAX_CELL_TEMP_HIGH);
    uint16_t bar_color = temperature_bar_color(bar_percentage);

    display_update_bar(&max_temp_bar, buf, bar_percentage, bar_color);
}

void update_race_pack_v_bar()
{
    char buf[10];
    snprintf(buf, sizeof(buf), "%u V", race_dash_data.hvbms_pack_v / 10);

    uint16_t bar_percentage = calculate_percentage(race_dash_data.hvbms_pack_v, PACK_V_LOW, PACK_V_HIGH);
    uint16_t bar_color = voltage_bar_color(bar_percentage);

    display_update_bar(&pack_v_bar, buf, bar_percentage, bar_color);
}

void update_race_motor_temp_bar()
{
    char buf[10];
    snprintf(buf, sizeof(buf), "%u.%01u C", race_dash_data.max_motor_temp / 10, race_dash_data.max_motor_temp % 10);

    uint16_t bar_percentage = calculate_percentage(race_dash_data.max_motor_temp, MOTOR_TEMP_LOW, MOTOR_TEMP_HIGH);
    uint16_t bar_color = temperature_bar_color(bar_percentage);

    display_update_bar(&motor_temp_bar, buf, bar_percentage, bar_color);
}

void update_race_inv_temp_bar()
{
    char buf[10];
    snprintf(buf, sizeof(buf), "%u.%01u C", race_dash_data.max_inv_temp / 10, race_dash_data.max_inv_temp % 10);

    uint16_t bar_percentage = calculate_percentage(race_dash_data.max_inv_temp, INV_TEMP_LOW, INV_TEMP_HIGH);
    uint16_t bar_color = temperature_bar_color(bar_percentage);

    display_update_bar(&inv_temp_bar, buf, bar_percentage, bar_color);
}
