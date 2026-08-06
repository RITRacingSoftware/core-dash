#include "dash.h"

#include <stdint.h>
#include <stdbool.h>

#include "core.h"

#include "config.h"

#include "appCAN.h"
#include "appGPIO.h"
#include "display.h"
#include "peripherals.h"
#include "appCAN.h"
#include "DataManager.h"
#include "debug_screen.h"
#include "race_screen.h"
#include "neopixel.h"
#include "controls_screen.h"

typedef enum
{
    SCREEN_DEBUG = 0,
    SCREEN_RACE,
    SCREEN_CONTROLS,
} screen_id_t;

static screen_id_t current_screen = SCREEN_DEBUG;
static uint8_t last_screen_selection = SCREEN_DEBUG;

void dash_update(void)
{
    uint8_t poll = enc2.current_poll;

    if (poll != last_screen_selection)
    {
        dash_data.screen_flag = true;
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
        else if (poll == 2){
            current_screen = SCREEN_CONTROLS;
            draw_controls_screen();
        }
    }

    if (current_screen == SCREEN_DEBUG) update_debug_screen();
    else if (current_screen == SCREEN_RACE) update_race_screen();
    dash_data.screen_flag = false;
}


bool dash_init() {
    if (!core_clock_init()) return false;
    core_timestamp_init();
    core_heartbeat_init(LED1_PORT, LED1_PIN);
    core_RTT_init();
    GPIO_init();
    encoders_init();    
    neopixel_init(); soc_startup();
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
    return true;
}

bool soc_bar_task()
{
    if (dash_data.endurance_flag)
    {
        update_endurance_neopixels(dash_data.endurance_delta);
    }

    return true;
}

