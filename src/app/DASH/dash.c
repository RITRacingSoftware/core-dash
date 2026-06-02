#include "dash.h"

#include <stdint.h>
#include <stdbool.h>

#include "clock.h"
#include "rtt.h"
#include "gpio.h"
#include "boot.h"

#include "config.h"

#include "appCAN.h"
#include "appGPIO.h"
#include "display.h"
#include "inputs.h"
#include "appCAN.h"
#include "DataManager.h"
#include "debug_screen.h"
#include "race_screen.h"

typedef enum
{
    SCREEN_DEBUG = 0,
    SCREEN_RACE,
} screen_id_t;

static screen_id_t current_screen = SCREEN_DEBUG;
static uint8_t last_screen_selection = 255;

void dash_update(void)
{
    uint8_t poll = enc2.current_poll;

    if (poll != last_screen_selection && sec_bus.vc_status.vc_status_vehicle_state != READY_TO_DRIVE_STATE)
    {
        // dash_data.screen_flag = true;
        last_screen_selection = poll;

        if (poll == 0)
        {
            current_screen = SCREEN_DEBUG;
            draw_debug_screen();
        }
        else if (poll == 1)
        {
            current_screen = SCREEN_RACE;
            draw_race_screen();
        }
    }

    if (current_screen == SCREEN_DEBUG) update_debug_screen();
    else if (current_screen == SCREEN_RACE) update_race_screen();
}


bool dash_init() {
    if (!core_clock_init()) return false;
    core_heartbeat_init(LED1_PORT, LED1_PIN);
    core_RTT_init();
    GPIO_init();
    encoders_init();
    display_init();
    if(!CAN_init()) return false;
    core_boot_init();


    return true;
}


bool dash_task()
{
    dash_update();
    return true;
}

bool inputs_task()
{
    CAN_send_input_data();
    // rprintf("Button #1: %u\n", sec_bus.inputs.dash_button1);
    return true;
}

