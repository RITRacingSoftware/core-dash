#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stm32g4xx_hal.h>

#define NUM_ENCODERS 3

// Encoder 1
#define ENC1_MIN 0
#define ENC1_MAX 8

// Encoder 2
#define ENC2_MIN 0
#define ENC2_MAX 8

// Encoder 3
#define ENC3_MIN 0
#define ENC3_MAX 8


typedef struct {
    GPIO_TypeDef *port_A;
    uint16_t pin_A;

    GPIO_TypeDef *port_B;
    uint16_t pin_B;

    GPIO_TypeDef *port_SW;
    uint16_t pin_SW;

    uint8_t position;
    uint8_t old_state;
    uint8_t current_poll;
    uint8_t min_rotation;
    uint8_t max_rotation;

} encoder_t;


void encoder_interrupt(encoder_t *enc);

bool encoder_insert(encoder_t *enc);

void encoders_init();

static uint8_t encoder_read_state(encoder_t *enc);



