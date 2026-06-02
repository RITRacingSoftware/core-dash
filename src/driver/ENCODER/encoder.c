#include "encoder.h"

#include <stdint.h>
#include <stdbool.h>

#include "gpio.h"
#include "rtt.h"

#include "appGPIO.h"

#define QUADRATURE 4

static const int8_t counter_lut[16] = 
{
    // starting @ 00
     0, -1, +1,  0,

    // starting @ 01
    +1,  0,  0, -1,

    // starting @ 10
    -1,  0,  0, +1,

    // starting @ 11
     0, +1, -1,  0
};


void encoder_interrupt(encoder_t *enc)
{
    uint8_t new_state = encoder_read_state(enc);
    uint8_t index = (enc->old_state << 2) | new_state;

    enc->position += counter_lut[index];
    enc->old_state = new_state;    

    int32_t min_count = enc->min_rotation * QUADRATURE;
    int32_t max_count = enc->max_rotation * QUADRATURE;

    if (enc->position < min_count) enc->position = min_count;
    if (enc->position > max_count) enc->position = max_count;

    enc->current_poll = enc->position / QUADRATURE;
    // rprintf("current poll: %u\n", enc->current_poll);
}
    

uint8_t encoder_read_state(encoder_t *enc)
{
    return (core_GPIO_digital_read(enc->port_A, enc->pin_A) << 1) |
            core_GPIO_digital_read(enc->port_B, enc->pin_B);
}

