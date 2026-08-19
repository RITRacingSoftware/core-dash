#include "display.h"
#include "RA8875.h"
#include "rtt.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32g4xx_hal.h"

#define BOX_SPACING_X 15
#define BOX_SPACING_Y 15

#define STATUS_X 0
#define STATUS_Y 0
#define STATUS_W SCREEN_WIDTH
#define STATUS_H 50
#define STATUS_TEXT_SCALE 2


#define BTN_WIDTH     200
#define BTN_HEIGHT    50
#define BTN_START_X   100
#define BTN_START_Y   100
#define BTN_SPACING   20
#define BTN_RADIUS    10

enum {
   DRS_ROW,
   VELOCITY_LIMIT_ROW 
};


static const display_row_t event_parameters_rows[] = {
    {"DRS", 4},
    {"VELOCITY LIMIT", 4}
};

static display_box_t event_parameters_box = {
    .title = "PARAMETERS",
    .title_h = 20,
    .x = 0,
    .y = 0,

    .margin_x = 12,
    .margin_y = 7,

    .label_value_gap = 10,
    .row_gap = 5,

    .rows = event_parameters_rows,
    .row_count = 2
};

static display_button_t accel_button = {
    .title = "ACCELERATION",
    .spacing = 20,

    .x = 0,
    .y = 0,
    .w = 0,
    .h = 0,
    .r = 0,

    .state = 0
};

static display_button_t autocross_button = {
    .title = "AUTOCROSS",
    .spacing = 20,

    .x = 0,
    .y = 0,
    .w = 0,
    .h = 0,
    .r = 0,

    .state = 0
};

/*
static display_button_t skidpad_button = {
    .title = "SKIDPAD",
    .spacing = 20,

    .x = 0,
    .y = 0,
    .w = 0,
    .h = 0,
    .r = 0,

    .state = 0
};


static display_button_t endurance_button = {
    .title = "ENDURANCE",
    .spacing = 20,

    .x = 0,
    .y = 0,
    .w = 0,
    .h = 0,
    .r = 0,

    .state = 0
};

static display_button_t off_button = {
    .title = "OFF",
    .spacing = 20,

    .x = 0,
    .y = 0,
    .w = 0,
    .h = 0,
    .r = 0,

    .state = 0
};*/



void draw_controls_screen(void){
    RA8875_graphic_mode();
    RA8875_fill_screen(RA8875_BLACK);
   
    //SCREEN STATUS
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

    //PARAMETER LOG
    event_parameters_box.y = STATUS_H + BOX_SPACING_Y;
    display_draw_box(&event_parameters_box, RA8875_WHITE, RA8875_BLACK);
    display_draw_box_labels(&event_parameters_box, RA8875_WHITE);

    display_update_box_value(&event_parameters_box, DRS_ROW, "??.?", RA8875_WHITE, RA8875_BLACK);
    display_update_box_value(&event_parameters_box, VELOCITY_LIMIT_ROW, "??.?", RA8875_WHITE, RA8875_BLACK);
    

    //ACCEL BUTTON
    accel_button.x = display_box_width(&event_parameters_box) + BOX_SPACING_Y;
    accel_button.y = event_parameters_box.y + display_box_height(&event_parameters_box) + BOX_SPACING_Y;
    accel_button.h = BTN_HEIGHT;
    accel_button.w = BTN_WIDTH;
    accel_button.r = BTN_RADIUS;
    accel_button.spacing = BTN_SPACING;
    display_draw_button(&accel_button, RA8875_WHITE, RA8875_BLACK);

    //AUTOCROSS BUTTON
    autocross_button.x = accel_button.x + BOX_SPACING_X;
    autocross_button.y = accel_button.y;
    autocross_button.h = BTN_HEIGHT;
    autocross_button.w = BTN_WIDTH;
    autocross_button.r = BTN_RADIUS;
    autocross_button.spacing = BTN_SPACING;
    display_draw_button(&autocross_button, RA8875_WHITE, RA8875_BLACK);

/*
    //SKIDPAD BUTTON
    skidpad_button.y = event_parameters_box.y + display_box_height(&event_parameters_box) + BOX_SPACING_Y;
    skidpad_button.h = BTN_HEIGHT;
    skidpad_button.w = BTN_WIDTH;
    skidpad_button.r = BTN_RADIUS;
    skidpad_button.spacing = BTN_SPACING;
    display_draw_button(&skidpad_button, RA8875_WHITE, RA8875_BLACK);


    //ENDURANCE BUTTON
    endurance_button.y = event_parameters_box.y + display_box_height(&event_parameters_box) + BOX_SPACING_Y;
    endurance_button.h = BTN_HEIGHT;
    endurance_button.w = BTN_WIDTH;
    endurance_button.r = BTN_RADIUS;
    endurance_button.spacing = BTN_SPACING;
    display_draw_button(&endurance_button, RA8875_WHITE, RA8875_BLACK);



    //OFF BUTTON
    off_button.y = event_parameters_box.y + display_box_height(&event_parameters_box) + BOX_SPACING_Y;
    off_button.h = BTN_HEIGHT;
    off_button.w = BTN_WIDTH;
    off_button.r = BTN_RADIUS;
    off_button.spacing = BTN_SPACING;
    display_draw_button(&off_button, RA8875_WHITE, RA8875_BLACK);
        
*/

    
}






