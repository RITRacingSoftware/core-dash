#include "encoder.h"

#include <stdint.h>
#include <stdbool.h>

#include "gpio.h"
#include "rtt.h"

#include "appGPIO.h"


static encoder_t enc1;
static encoder_t enc2;
static encoder_t enc3;

static encoder_t *encoder_list[NUM_ENCODERS];
static uint8_t n_encoders = 0;

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
}
    

bool encoder_insert(encoder_t *enc)
{
    if (n_encoders < NUM_ENCODERS) {
        encoder_list[n_encoders] = enc;
        n_encoders++;
        return true;
    } 
    return false;
}


void encoders_init()
{

    // ENCODER 1
    enc1.port_A = EN1A_PORT;
    enc1.pin_A = EN1A_PIN;
    enc1.port_B = EN1B_PORT;
    enc1.pin_B  = EN1B_PIN;
    enc1.port_SW = PBEN1_PORT;
    enc1.pin_SW = PBEN1_PIN;
    enc1.position = 0;  // REPLACE WITH EEPROM
    enc1.old_state = encoder_read_state(&enc1);
    enc1.min_rotation = ENC1_MIN;
    enc1.max_rotation = ENC1_MAX;
    encoder_insert(&enc1);

    // ENCODER 2
    enc2.port_A = EN2A_PORT;
    enc2.pin_A  = EN2A_PIN;
    enc2.port_B = EN2B_PORT;
    enc2.pin_B  = EN2B_PIN;
    enc2.port_SW = PBEN2_PORT;    
    enc2.pin_SW  = PBEN2_PIN;
    enc2.position = 0;  // REPLACE WITH EEPROM
    enc2.old_state = encoder_read_state(&enc2);             
    enc2.min_rotation = ENC2_MIN;
    enc2.max_rotation = ENC2_MAX;
    encoder_insert(&enc2);

    // ENCODER 3
    enc3.port_A = EN3A_PORT;
    enc3.pin_A  = EN3A_PIN;
    enc3.port_B = EN3B_PORT;
    enc3.pin_B  = EN3B_PIN;
    enc3.port_SW = PBEN3_PORT;    
    enc3.pin_SW  = PBEN3_PIN;
    enc3.position = 0;  // REPLACE WITH EEPROM
    enc3.old_state = encoder_read_state(&enc3);       
    enc3.min_rotation = ENC3_MIN;
    enc3.max_rotation = ENC3_MAX;
    encoder_insert(&enc3);

}

static uint8_t encoder_read_state(encoder_t *enc)
{
    return (core_GPIO_digital_read(enc->port_A, enc->pin_A) << 1) |
            core_GPIO_digital_read(enc->port_B, enc->pin_B);
}

   






