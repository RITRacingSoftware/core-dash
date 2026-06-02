#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stm32g4xx_hal.h>


typedef struct {
    GPIO_TypeDef *port_A;
    uint16_t pin_A;

    GPIO_TypeDef *port_B;
    uint16_t pin_B;

    GPIO_TypeDef *port_SW;
    uint16_t pin_SW;

    int32_t position;
    uint8_t old_state;
    uint8_t current_poll;
    uint8_t min_rotation;
    uint8_t max_rotation;

} encoder_t;

void encoder_interrupt(encoder_t *enc);
uint8_t encoder_read_state(encoder_t *enc);


