#include "debug_screen.h"
#include "display.h"
#include "DataManager.h"
#include "RA8875.h"
#include "rtt.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32g4xx_hal.h"

#define BOX_SPACING_X 15
#define BOX_SPACING_Y 15

#define STATUS_X          0
#define STATUS_Y          0
#define STATUS_W          SCREEN_WIDTH
#define STATUS_H          50
#define STATUS_TEXT_SCALE 2

#define FAULT_X 314
#define FAULT_Y 65
#define FAULT_W 166
#define FAULT_H 163

#define FAULT_TITLE_H    20
#define FAULT_MARGIN_X   12
#define FAULT_MARGIN_Y   5
#define FAULT_ROW_GAP    3
#define FAULT_TEXT_SCALE 0

#define MAX_FAULTS        VC_FAULT_COUNT + PDU_FAULT_COUNT + HVBMS_FAULT_COUNT + LVBMS_FAULT_COUNT
#define VC_FAULT_COLOR    RA8875_CYAN
#define PDU_FAULT_COLOR   RA8875_MAGENTA
#define HVBMS_FAULT_COLOR RA8875_ORANGE
#define LVBMS_FAULT_COLOR RA8875_GREEN

#define NOT_READY_STATE_COLOR         RA8875_RED
#define INVERTERS_POWERED_STATE_COLOR RA8875_CYAN
#define PRECHARGING_STATE_COLOR       RA8875_ORANGE
#define WAIT_STATE_COLOR              RA8875_YELLOW
#define STANDBY_STATE_COLOR           RA8875_WHITE
#define READY_TO_DRIVE_STATE_COLOR    RA8875_GREEN
#define SHUTDOWN_STATE_COLOR          RA8875_RED

#define CONTROLS_TEXT_SCALE 0
#define CONTROLS_X FAULT_X
#define CONTROLS_Y 243
#define CONTROLS_W FAULT_W
#define CONTROLS_H 25

#define CONTROLS_LEVEL_OFF       0
#define CONTROLS_LEVEL_BASIC     1
#define CONTROLS_LEVEL_BASIC_VEL 2
#define CONTROLS_LEVEL_ADVANCED  3
#define CONTROLS_LEVEL_SKIDPAD   4

#define CONTROLS_LEVEL_OFF_COLOR       RA8875_RED
#define CONTROLS_LEVEL_BASIC_COLOR     RA8875_ORANGE 
#define CONTROLS_LEVEL_BASIC_VEL_COLOR RA8875_YELLOW
#define CONTROLS_LEVEL_ADVANCED_COLOR  RA8875_GREEN
#define CONTROLS_LEVEL_SKIDPAD_COLOR   RA8875_CYAN

#define MIN_CELL_VOLT_IRR_HIGH 450  // 4.50 V
#define MIN_CELL_VOLT_YELLOW   380  // 3.80 V
#define MIN_CELL_VOLT_RED      345  // 3.45 V
#define MIN_CELL_VOLT_IRR_LOW  200  // 2.00 V

#define MAX_CELL_TEMP_IRR_HIGH 700  // 70.0 C
#define MAX_CELL_TEMP_YELLOW   400  // 40.0 C
#define MAX_CELL_TEMP_RED      500  // 50.0 C
#define MAX_CELL_TEMP_IRR_LOW  10   //  1.0 C

#define MOTOR_TEMP_IRR_HIGH    2000 //  200 C
#define MOTOR_TEMP_YELLOW      950  //   95 C
#define MOTOR_TEMP_RED         1150 //  115 C
#define MOTOR_TEMP_IRR_LOW     10   //  1.0 C

#define INV_TEMP_IRR_HIGH      700  //   70 C
#define INV_TEMP_YELLOW        400  //   40 C 
#define INV_TEMP_RED           500  //   50 C
#define INV_TEMP_IRR_LOW       10   //  1.0 C

#define LV_PACK_IRR_HIGH MIN_CELL_VOLT_IRR_HIGH * 6
#define LV_PACK_YELLOW   MIN_CELL_VOLT_YELLOW * 6
#define LV_PACK_RED      MIN_CELL_VOLT_RED * 6
#define LV_PACK_IRR_LOW  MIN_CELL_VOLT_IRR_LOW * 6

#define HV_PACK_IRR_HIGH MIN_CELL_VOLT_IRR_HIGH * 135 / 10
#define HV_PACK_YELLOW   MIN_CELL_VOLT_YELLOW * 135 / 10
#define HV_PACK_RED      MIN_CELL_VOLT_RED * 135 / 10
#define HV_PACK_IRR_LOW  MIN_CELL_VOLT_IRR_LOW * 135 / 10

enum
{
    HVBMS_MIN_CELL_ROW = 0,
    HVBMS_MAX_TEMP_ROW,
    HVBMS_PACK_V_ROW
};

enum
{
    LVBMS_MIN_CELL_ROW = 0,
    LVBMS_MAX_TEMP_ROW,
    LVBMS_PACK_V_ROW
};

enum
{
    FR_MOTOR_TEMP_ROW = 0,
    FL_MOTOR_TEMP_ROW,
    RR_MOTOR_TEMP_ROW,
    RL_MOTOR_TEMP_ROW
};

enum
{
    INV_TEMP_AVG_ROW = 0,
    INV_TEMP_MAX_ROW
};

static const char *faults[MAX_FAULTS] = {0};
static uint8_t vc_fault_count = 0;
static uint8_t pdu_fault_count = 0;
static uint8_t hvbms_fault_count = 0;
static uint8_t lvbms_fault_count = 0;

static const display_row_t hv_bms_rows[] =
{
    {"Min Cell:", 7},
    {"Max Temp:", 7},
    {"Pack V:",   7},
};

static display_box_t hv_bms_box =
{
    .title = "HV BMS",

    .x = 0,
    .y = 0,

    .title_h = 20,

    .margin_x = 12,
    .margin_y = 7,

    .label_value_gap = 10,
    .row_gap = 5,

    .rows = hv_bms_rows,
    .row_count = 3
};

static const display_row_t lv_bms_rows[] =
{
    {"Min Cell:", 7},
    {"Max Temp:", 7},
    {"Pack V:",   7},
};

static display_box_t lv_bms_box =
{
    .title = "LV BMS",

    .x = 0,
    .y = 0,

    .title_h = 20,

    .margin_x = 12,
    .margin_y = 7,

    .label_value_gap = 10,
    .row_gap = 5,

    .rows = lv_bms_rows,
    .row_count = 3
};

static const display_row_t motor_rows[] =
    {
        {"FR:", 7},
        {"FL:", 7},
        {"RR:", 7},
        {"RL:", 7}
    };

static display_box_t motor_box =
{
    .title = "MOTORS",

    .x = 0,
    .y = 0,

    .title_h = 20,

    .margin_x = 16,
    .margin_y = 7,

    .label_value_gap = 10,
    .row_gap = 5,

    .rows = motor_rows,
    .row_count = 4
};

static const display_row_t inverter_row[] =
{
    {"Avg:", 7},
    {"Max:", 7}
};

static display_box_t inverter_box =
{
    .title = "INVERTERS",

    .x = 0,
    .y = 0,

    .title_h = 20,

    .margin_x = 12,
    .margin_y = 7,

    .label_value_gap = 10,
    .row_gap = 5,

    .rows = inverter_row,
    .row_count = 2
};


static void pack_faults()
{
    uint8_t fault_index = 0;

    vc_fault_count = 0;
    pdu_fault_count = 0;
    hvbms_fault_count = 0;
    lvbms_fault_count = 0;

    memset(faults, 0, sizeof(faults));

    for (int i = 0; i < VC_FAULT_COUNT && fault_index < MAX_FAULTS; i++)
    {
        if ((debug_dash_data.vc_fault_vector >> i) & 1)
        {
            faults[fault_index++] = VC_FAULT_MESSAGES[i];
            vc_fault_count++;
        }
    }

    for (int i = 0; i < PDU_FAULT_COUNT && fault_index < MAX_FAULTS; i++)
    {
        if ((debug_dash_data.pdu_fault_vector >> (32 + i)) & 1)
        {
            faults[fault_index++] = PDU_FAULT_MESSAGES[i];
            pdu_fault_count++;
        }
    }

    for (int i = 0; i < HVBMS_FAULT_COUNT && fault_index < MAX_FAULTS; i++)
    {
        if ((debug_dash_data.hvbms_fault_vector >> i) & 1)
        {
            faults[fault_index++] = HVBMS_FAULT_MESSAGES[i];
            hvbms_fault_count++;
        }
    }

    for (int i = 0; i < LVBMS_FAULT_COUNT && fault_index < MAX_FAULTS; i++)
    {
        if ((debug_dash_data.lvbms_fault_vector >> i) & 1)
        {
            faults[fault_index++] = LVBMS_FAULT_MESSAGES[i];
            lvbms_fault_count++;
        }
    }
}

static uint16_t min_cell_volt_color (uint16_t min_cell_volt)
{
    uint16_t text_color;

    if (min_cell_volt > MIN_CELL_VOLT_YELLOW) text_color = RA8875_GREEN;
    else if (min_cell_volt > MIN_CELL_VOLT_RED) text_color = RA8875_YELLOW;
    else text_color = RA8875_RED;
    return text_color;
}

static bool min_cell_volt_irr (uint16_t min_cell_volt)
{
    if (min_cell_volt > MIN_CELL_VOLT_IRR_HIGH || min_cell_volt < MIN_CELL_VOLT_IRR_LOW) return true;
    else return false;
}

static uint16_t max_cell_temp_color (uint16_t max_cell_temp)
{
    uint16_t text_color;

    if (max_cell_temp < MAX_CELL_TEMP_YELLOW) text_color = RA8875_GREEN;
    else if (max_cell_temp < MAX_CELL_TEMP_RED) text_color = RA8875_YELLOW;
    else text_color = RA8875_RED;
    return text_color;
}

static bool max_cell_temp_irr (uint16_t max_cell_temp)
{
    if (max_cell_temp > MAX_CELL_TEMP_IRR_HIGH || max_cell_temp < MAX_CELL_TEMP_IRR_LOW) return true;
    else return false;
}

static uint16_t motor_temp_color (uint16_t motor_temp)
{
    uint16_t text_color;
    if (motor_temp < MOTOR_TEMP_YELLOW) text_color = RA8875_GREEN;
    else if (motor_temp < MOTOR_TEMP_RED) text_color = RA8875_YELLOW;
    else text_color = RA8875_RED;
    return text_color;
}

static bool motor_temp_irr (uint16_t motor_temp)
{
    if (motor_temp > MOTOR_TEMP_IRR_HIGH || motor_temp < MOTOR_TEMP_IRR_LOW) return true;
    else return false;
}

static uint16_t inv_temp_color (uint16_t inv_temp)
{ 
    uint16_t text_color;
    if (inv_temp < INV_TEMP_YELLOW) text_color = RA8875_GREEN;
    else if (inv_temp < INV_TEMP_RED) text_color = RA8875_YELLOW;
    else text_color = RA8875_RED;
    return text_color;
}

static bool inv_temp_irr (uint16_t inv_temp)
{
    if (inv_temp > INV_TEMP_IRR_HIGH || inv_temp < INV_TEMP_IRR_LOW) return true;
    else return false;
}

static uint16_t lv_pack_color (uint16_t lv_pack_v)
{
    uint16_t text_color;
    if (lv_pack_v > LV_PACK_YELLOW) text_color = RA8875_GREEN;
    else if (lv_pack_v > LV_PACK_RED) text_color = RA8875_YELLOW;
    else text_color = RA8875_RED;
    return text_color;
}

static bool lv_pack_irr (uint16_t lv_pack_v)
{
    if (lv_pack_v > LV_PACK_IRR_HIGH || lv_pack_v < LV_PACK_IRR_LOW) return true;
    else return false;
}

static uint16_t hv_pack_color (uint16_t hv_pack_v)
{
    uint16_t text_color;
    if (hv_pack_v > HV_PACK_YELLOW) text_color = RA8875_GREEN;
    else if (hv_pack_v > HV_PACK_RED) text_color = RA8875_YELLOW;
    else text_color = RA8875_RED;
    return text_color;
}

static bool hv_pack_irr (uint16_t hv_pack_v)
{
    if (hv_pack_v > HV_PACK_IRR_HIGH || hv_pack_v < HV_PACK_IRR_LOW) return true;
    else return false;
}


void draw_debug_screen(void)
{

    RA8875_graphic_mode();
    RA8875_fill_screen(RA8875_BLACK);

//--------------------STATUS--------------------//    
    
    display_status_banner(
        STATUS_X,                  
        STATUS_Y,                  
        SCREEN_WIDTH,              
        STATUS_H,                  
        STATUS_TEXT_SCALE,                                
        "NOT READY",
        RA8875_RED,
        RA8875_BLACK
    );


//--------------------HV BMS--------------------//

    hv_bms_box.y = STATUS_H + BOX_SPACING_Y;

    display_draw_box(&hv_bms_box, RA8875_WHITE, RA8875_BLACK);
    display_draw_box_labels(&hv_bms_box, RA8875_WHITE);

    display_update_box_value(&hv_bms_box, HVBMS_MIN_CELL_ROW, "?.?? V", RA8875_WHITE, RA8875_BLACK);
    display_update_box_value(&hv_bms_box, HVBMS_MAX_TEMP_ROW, "??.? C", RA8875_WHITE, RA8875_BLACK);
    display_update_box_value(&hv_bms_box, HVBMS_PACK_V_ROW, " ??? V", RA8875_WHITE, RA8875_BLACK); 


//--------------------LV BMS--------------------//

    lv_bms_box.y = hv_bms_box.y + display_box_height(&hv_bms_box) + BOX_SPACING_Y;

    display_draw_box(&lv_bms_box, RA8875_WHITE, RA8875_BLACK);
    display_draw_box_labels(&lv_bms_box, RA8875_WHITE);

    display_update_box_value(&lv_bms_box, LVBMS_MIN_CELL_ROW, "?.?? V", RA8875_WHITE, RA8875_BLACK);
    display_update_box_value(&lv_bms_box, LVBMS_MAX_TEMP_ROW, "??.? C", RA8875_WHITE, RA8875_BLACK);
    display_update_box_value(&lv_bms_box, LVBMS_PACK_V_ROW, "??.? V", RA8875_WHITE, RA8875_BLACK); 


    //--------------------MOTORS--------------------//

    
    motor_box.x = display_box_width(&hv_bms_box) + BOX_SPACING_X;
    motor_box.y = hv_bms_box.y;

    display_draw_box(&motor_box, RA8875_WHITE, RA8875_BLACK);
    display_draw_box_labels(&motor_box, RA8875_WHITE);

    display_update_box_value(&motor_box, FR_MOTOR_TEMP_ROW, "???.? C", RA8875_WHITE, RA8875_BLACK);
    display_update_box_value(&motor_box, FL_MOTOR_TEMP_ROW, "???.? C", RA8875_WHITE, RA8875_BLACK);
    display_update_box_value(&motor_box, RR_MOTOR_TEMP_ROW, "???.? C", RA8875_WHITE, RA8875_BLACK);
    display_update_box_value(&motor_box, RL_MOTOR_TEMP_ROW, "???.? C", RA8875_WHITE, RA8875_BLACK);


    //--------------------INVERTERS--------------------//

    inverter_box.x = motor_box.x;
    inverter_box.y = motor_box.y + display_box_height(&motor_box) + BOX_SPACING_Y;

    display_draw_box(&inverter_box, RA8875_WHITE, RA8875_BLACK);
    display_draw_box_labels(&inverter_box, RA8875_WHITE);

    display_update_box_value(&inverter_box, INV_TEMP_AVG_ROW, "??.? C", RA8875_WHITE, RA8875_BLACK);
    display_update_box_value(&inverter_box, INV_TEMP_MAX_ROW, "??.? C", RA8875_WHITE, RA8875_BLACK);
    
  
    //------------------FAULT LOG------------------//  

    uint16_t fault_x = 2*BOX_SPACING_X + display_box_width(&hv_bms_box) + display_box_width(&motor_box);
    uint16_t fault_y = motor_box.y;
    uint16_t fault_w = SCREEN_WIDTH - 2*BOX_SPACING_X - display_box_width(&hv_bms_box) - display_box_width(&motor_box);
    // uint16_t fault_h = BOX_SPACING_Y + display_box_height(&motor_box) + display_box_height(&inverter_box);
    uint16_t fault_h = (display_box_height(&motor_box) + display_box_height(&inverter_box)) - CONTROLS_H;

    // rprintf("FAULT X: %u\n", fault_x);
    // rprintf("FAULT Y: %u\n", fault_y);
    // rprintf("FAULT W: %u\n", fault_w);
    // rprintf("FAULT H: %u\n", fault_h);

    display_fault_log(
        fault_x,
        fault_y,
        fault_w,
        fault_h,
        FAULT_TITLE_H,
        FAULT_MARGIN_X,
        FAULT_MARGIN_Y,
        FAULT_ROW_GAP,
        FAULT_TEXT_SCALE,
        "FAULT LOG",
        faults,
        0,
        0,
        0,                  
        0,                  
        VC_FAULT_COLOR,  
        PDU_FAULT_COLOR,
        HVBMS_FAULT_COLOR,         
        LVBMS_FAULT_COLOR,       
        RA8875_WHITE,      
        RA8875_BLACK        
    );


    //---------------CONTROLS LEVEL---------------//  

    uint16_t controls_x = fault_x;
    uint16_t controls_y = STATUS_H + 2 * BOX_SPACING_Y + fault_h;
    uint16_t controls_w = fault_w;
    
    // rprintf("CONTROLS X: %u\n", controls_x);
    // rprintf("CONTROLS Y: %u\n", controls_y);
    // rprintf("CONTROLS W: %u\n", controls_w);


    display_status_banner(
        controls_x,                  
        controls_y,                  
        controls_w,              
        CONTROLS_H,                  
        CONTROLS_TEXT_SCALE,                                
        "CONTROLS LEVEL",
        RA8875_WHITE,
        RA8875_BLACK
    );


    HAL_Delay(500); 
}



void update_debug_screen(void)
{

    DataManager_update_debug_data();
    
    if (debug_dash_data.hvbms_flag)
    {
        update_debug_hvbms_box();
        debug_dash_data.hvbms_flag = false;
    }

    if (debug_dash_data.lvbms_flag)
    {
        update_debug_lvbms_box();
        debug_dash_data.lvbms_flag = false;
    }

    if (debug_dash_data.motor_flag)
    {
        update_debug_motor_box();
        debug_dash_data.motor_flag = false;
    }

    if (debug_dash_data.inverter_flag)
    {
        update_debug_inverter_box();
        debug_dash_data.inverter_flag = false;
    }

    if (debug_dash_data.fault_flag)
    {
        update_debug_fault_log();
        debug_dash_data.fault_flag = false;
    }

    if (debug_dash_data.vc_status_flag)
    {
        update_debug_status_bar();
        debug_dash_data.vc_status_flag = false;
    }

    if (debug_dash_data.vc_controls_level_flag)
    {
        update_debug_controls_level();
        debug_dash_data.vc_controls_level_flag = false;
    }
}


void update_debug_hvbms_box()
{
    char buf[16];

    // HV MIN CELL VOLT

    if (min_cell_volt_irr(debug_dash_data.hvbms_min_cell))
    {
        snprintf(buf, sizeof(buf), "-.-- V");
        display_update_box_value(&hv_bms_box, HVBMS_MIN_CELL_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    } 

    else
    { 
        snprintf(buf, sizeof(buf), "%u.%02u V", debug_dash_data.hvbms_min_cell / 100, debug_dash_data.hvbms_min_cell % 100);
        display_update_box_value(&hv_bms_box, HVBMS_MIN_CELL_ROW, buf, min_cell_volt_color(debug_dash_data.hvbms_min_cell), RA8875_BLACK);
    }

    // HV MAX CELL TEMP

    if (max_cell_temp_irr(debug_dash_data.hvbms_max_temp))
    {
        snprintf(buf, sizeof(buf), "--.- C");
        display_update_box_value(&hv_bms_box, HVBMS_MAX_TEMP_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {    
        snprintf(buf, sizeof(buf), "%2u.%01u C", debug_dash_data.hvbms_max_temp / 10, debug_dash_data.hvbms_min_cell % 10);
        display_update_box_value(&hv_bms_box, HVBMS_MAX_TEMP_ROW, buf, max_cell_temp_color(debug_dash_data.hvbms_max_temp), RA8875_BLACK);
    }  

    // HV PACK VOLTAGE
    
    if (hv_pack_irr(debug_dash_data.hvbms_pack_v))
    {
        snprintf(buf, sizeof(buf), " --- V");
        display_update_box_value(&hv_bms_box, HVBMS_PACK_V_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%4u V", debug_dash_data.hvbms_pack_v / 10);
        display_update_box_value(&hv_bms_box, HVBMS_PACK_V_ROW, buf, hv_pack_color(debug_dash_data.hvbms_pack_v), RA8875_BLACK);
    }
}


void update_debug_lvbms_box()
{
    char buf[16];

    // LV MIN CELL VOLT

    if (min_cell_volt_irr(debug_dash_data.lvbms_min_cell))
    {
        snprintf(buf, sizeof(buf), "-.-- V");
        display_update_box_value(&lv_bms_box, LVBMS_MIN_CELL_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%u.%02u V", debug_dash_data.lvbms_min_cell / 100, debug_dash_data.lvbms_min_cell % 100);
        display_update_box_value(&lv_bms_box, LVBMS_MIN_CELL_ROW, buf, min_cell_volt_color(debug_dash_data.lvbms_min_cell), RA8875_BLACK);
    }

    // LV MAX CELL TEMP

    if (max_cell_temp_irr(debug_dash_data.lvbms_max_temp))
    {
        snprintf(buf, sizeof(buf), "--.- C");
        display_update_box_value(&lv_bms_box, LVBMS_MAX_TEMP_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%2u.%01u C", debug_dash_data.lvbms_max_temp / 10, debug_dash_data.lvbms_max_temp % 10);
        display_update_box_value(&lv_bms_box, LVBMS_MAX_TEMP_ROW, buf, max_cell_temp_color(debug_dash_data.lvbms_max_temp), RA8875_BLACK);
    }

    // LV PACK VOLTAGE

    if (lv_pack_irr(debug_dash_data.lvbms_pack_v))
    {
        snprintf(buf, sizeof(buf), "--.- V");
        display_update_box_value(&lv_bms_box, LVBMS_PACK_V_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%2u.%01u V", debug_dash_data.lvbms_pack_v / 100, (debug_dash_data.lvbms_pack_v % 100) / 10);
        display_update_box_value(&lv_bms_box, LVBMS_PACK_V_ROW, buf, lv_pack_color(debug_dash_data.lvbms_pack_v), RA8875_BLACK);
    }
}


void update_debug_motor_box()
{
    char buf[16];

    // FR MOTOR TEMP

    if (motor_temp_irr(debug_dash_data.fr_motor_temp))
    {
        snprintf(buf, sizeof(buf), "---.- C");
        display_update_box_value(&motor_box, FR_MOTOR_TEMP_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%3u.%01u C", debug_dash_data.fr_motor_temp / 10, debug_dash_data.fr_motor_temp % 10);
        display_update_box_value(&motor_box, FR_MOTOR_TEMP_ROW, buf, motor_temp_color(debug_dash_data.fr_motor_temp), RA8875_BLACK);
    }

    // FL MOTOR TEMP

    if (motor_temp_irr(debug_dash_data.fl_motor_temp))
    {
        snprintf(buf, sizeof(buf), "---.- C");
        display_update_box_value(&motor_box, FL_MOTOR_TEMP_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%3u.%01u C", debug_dash_data.fl_motor_temp / 10, debug_dash_data.fl_motor_temp % 10);
        display_update_box_value(&motor_box, FL_MOTOR_TEMP_ROW, buf, motor_temp_color(debug_dash_data.fl_motor_temp), RA8875_BLACK);
    }

    // RR MOTOR TEMP

    if (motor_temp_irr(debug_dash_data.rr_motor_temp))
    {
        snprintf(buf, sizeof(buf), "---.- C");
        display_update_box_value(&motor_box, FR_MOTOR_TEMP_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%3u.%01u C", debug_dash_data.rr_motor_temp / 10, debug_dash_data.rr_motor_temp % 10);
        display_update_box_value(&motor_box, RR_MOTOR_TEMP_ROW, buf, motor_temp_color(debug_dash_data.rr_motor_temp), RA8875_BLACK);
    }

    // RL MOTOR TEMP

    if (motor_temp_irr(debug_dash_data.rl_motor_temp))
    {
        snprintf(buf, sizeof(buf), "---.- C");
        display_update_box_value(&motor_box, RL_MOTOR_TEMP_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%3u.%01u C", debug_dash_data.rl_motor_temp / 10, debug_dash_data.rl_motor_temp % 10);
        display_update_box_value(&motor_box, RL_MOTOR_TEMP_ROW, buf, motor_temp_color(debug_dash_data.rl_motor_temp), RA8875_BLACK);
    }
}


void update_debug_inverter_box()
{
    char buf[16];

    // AVG INVERTER TEMP

    if (inv_temp_irr(debug_dash_data.avg_inv_temp))
    {
        snprintf(buf, sizeof(buf), "--.- C");
        display_update_box_value(&inverter_box, INV_TEMP_AVG_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%2u.%01u C", debug_dash_data.avg_inv_temp / 10, debug_dash_data.avg_inv_temp % 10);
        display_update_box_value(&inverter_box, INV_TEMP_AVG_ROW, buf, inv_temp_color(debug_dash_data.avg_inv_temp), RA8875_BLACK); 
    }

    // MAX INVERTER TEMP

    if (inv_temp_irr(debug_dash_data.max_inv_temp))
    {
        snprintf(buf, sizeof(buf), "--.- C");
        display_update_box_value(&inverter_box, INV_TEMP_MAX_ROW, buf, RA8875_WHITE, RA8875_BLACK);
    }

    else
    {
        snprintf(buf, sizeof(buf), "%2u.%01u C", debug_dash_data.max_inv_temp / 10, debug_dash_data.max_inv_temp % 10);
        display_update_box_value(&inverter_box, INV_TEMP_MAX_ROW, buf, inv_temp_color(debug_dash_data.avg_inv_temp), RA8875_BLACK);
    }
}


void update_debug_fault_log()
{
    pack_faults();

    display_fault_log(
        FAULT_X,
        FAULT_Y,
        FAULT_W,
        FAULT_H,
        FAULT_TITLE_H,
        FAULT_MARGIN_X,
        FAULT_MARGIN_Y,
        FAULT_ROW_GAP,
        FAULT_TEXT_SCALE,
        "FAULT LOG",
        faults,
        vc_fault_count,
        pdu_fault_count,
        hvbms_fault_count,
        lvbms_fault_count,
        VC_FAULT_COLOR,
        PDU_FAULT_COLOR,
        HVBMS_FAULT_COLOR,      
        LVBMS_FAULT_COLOR,     
        RA8875_WHITE,    
        RA8875_BLACK
    ); 
}


void update_debug_status_bar()
{
    uint8_t state = debug_dash_data.vc_status.vc_status_vehicle_state;

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


void update_debug_controls_level()
{
    uint8_t level = debug_dash_data.vc_status.vc_controls_level;

    switch (level)
    {
        case CONTROLS_LEVEL_OFF :
            display_status_banner(
                CONTROLS_X,                  
                CONTROLS_Y,                  
                CONTROLS_W,              
                CONTROLS_H,                  
                CONTROLS_TEXT_SCALE,                                
                VC_CONTROLS_LEVELS[level],
                CONTROLS_LEVEL_OFF_COLOR,
                RA8875_BLACK
            ); break;

        case CONTROLS_LEVEL_BASIC :
            display_status_banner(
                CONTROLS_X,                  
                CONTROLS_Y,                  
                CONTROLS_W,            
                CONTROLS_H,                  
                CONTROLS_TEXT_SCALE,                                
                VC_CONTROLS_LEVELS[level],
                CONTROLS_LEVEL_BASIC_COLOR,
                RA8875_BLACK
            ); break;

        case CONTROLS_LEVEL_BASIC_VEL :
            display_status_banner(
                CONTROLS_X,                  
                CONTROLS_Y,                  
                CONTROLS_W,       
                CONTROLS_H,                  
                CONTROLS_TEXT_SCALE,                                
                VC_CONTROLS_LEVELS[level],
                CONTROLS_LEVEL_BASIC_VEL_COLOR,
                RA8875_BLACK
            ); break;

        case CONTROLS_LEVEL_ADVANCED :
            display_status_banner(
                CONTROLS_X,                  
                CONTROLS_Y,                  
                CONTROLS_W,       
                CONTROLS_H,                  
                CONTROLS_TEXT_SCALE,                                
                VC_CONTROLS_LEVELS[level],
                CONTROLS_LEVEL_ADVANCED_COLOR,
                RA8875_BLACK
            ); break;

        case CONTROLS_LEVEL_SKIDPAD :
            display_status_banner(
                CONTROLS_X,                  
                CONTROLS_Y,                  
                CONTROLS_W,                     
                CONTROLS_H,                  
                CONTROLS_TEXT_SCALE,                                
                VC_CONTROLS_LEVELS[level],
                CONTROLS_LEVEL_SKIDPAD_COLOR,
                RA8875_BLACK
            ); break;

        default: return;
    }
}
