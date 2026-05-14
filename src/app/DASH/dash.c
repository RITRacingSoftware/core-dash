#include "dash.h"

#include <stdint.h>
#include <stdbool.h>

#include "clock.h"
#include "rtt.h"
#include "gpio.h"

#include "config.h"

#include "appGPIO.h"
#include "display.h"

bool dash_init() {
    if (!core_clock_init()) return false;
    core_heartbeat_init(LED1_PORT, LED1_PIN);

    core_RTT_init();
    GPIO_init();
    display_init();
    return true;
}
