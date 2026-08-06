#pragma once

#include <stdint.h>
#include "encoder.h"

#define NUM_ENCODERS 3

// Encoder 1
#define ENC1_MIN 0
#define ENC1_MAX 8

// Encoder 2
#define ENC2_MIN 0
#define ENC2_MAX 2

// Encoder 3
#define ENC3_MIN 0
#define ENC3_MAX 8

extern encoder_t enc1;
extern encoder_t enc2;
extern encoder_t enc3;

void encoders_init(void);
void update_endurance_neopixels(int32_t delta);
void soc_startup(void);

