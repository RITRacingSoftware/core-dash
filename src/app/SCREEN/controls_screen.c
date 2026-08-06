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

#define MODE_X
#define MODE_Y
#define MODE_W
#define MODE_H

#define PARAMETERS_X
#define PARAMETERS_Y
#define PARAMETERS_W
#define PARAMETERS_H

static const display_row_t controls_rows[] = {
    {"Test", 7}
};

static display_box_t controls_param_box = {
    .title = "PARAMETERS",

    .x = 0,
    .y = 0,

    .title_h = 20,
    .margin_x = 16,
    .margin_y = 7,

    .label_value_gap = 10,
    .row_gap = 5,
    .rows = controls_rows,
    .row_count = 1

};




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

    //controls_param_box.x = 2*BOX_SPACING_X;
    //controls_param_box.y = STATUS_H + BOX_SPACING_Y;
    //CONTROLS PARAMETERS 
    display_draw_box(&controls_param_box, RA8875_WHITE, RA8875_BLACK);
    display_draw_box_labels(&controls_param_box, RA8875_WHITE);

}

/*
    //CONTROLS LEVEL
    uint16_t controls_x = fault_x
    uint16_t controls_y = STATUS_H + 2 * BOX_SPACING_Y + fault_h;
    uint16_t controls_w = fault_w

    display_status_banner(
        controls_x,
        controls_y,
        controls_w,
        CONTROLS_H,
        "CONTROLS LEVEL"
        RA8875_WHITE,
        RA8875_BLACK
    );

    //EBS STATE
    //uint16_t ebs_x = 
    //uint16_t ebs_y = 
    //uint16_t ebs_w = 

    
     *  display_status_banner(
     *      ebs_x,
     *      ebs_y,
     *      ebs_w,
     *      EBS_H,
     *      "EBS STATUS",
     *      RA8875_WHITE,
     *      RA8875_BLACK
     *  );
     *
     * 
}


void update_controls_driving_event(void){
    //uint8_t event = dash_data.vc_status.vc_driving_event;


    switch(event){
        case ENDURANCE: {

            break;
        }
            
        case AUTOCROSS: {

            break;
        }


        case SKIDPAD: {

            break;
        }


        case ACCEL: {

            break;
        }

    }

}



