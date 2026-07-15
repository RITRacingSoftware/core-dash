#pragma once
#include <stdint.h>

#define NEOPIXEL_COUNT 11

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} neopixel_t;

void neopixel_init(void);
void neopixel_set(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void neopixel_clear(void);
void neopixel_write(void);
void neopixel_delay_ms(uint32_t delay_ms);
