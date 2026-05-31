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
#include "DataManager.h"
#include "debug_screen.h"
#include "race_screen.h"

bool dash_init() {
    if (!core_clock_init()) return false;
    core_heartbeat_init(LED1_PORT, LED1_PIN);
    core_RTT_init();
    GPIO_init();
    DataManager_init();
    display_init();
    if(!CAN_init()) return false;
    core_boot_init();


    return true;
}

bool dash_task()
{
    update_debug_screen();
    // update_race_screen();
    return true;
}

bool inputs_task()
{
    CAN_send_input_data();
    // rprintf("Button #1: %u\n", sec_bus.inputs.dash_button1);
    return true;
}

