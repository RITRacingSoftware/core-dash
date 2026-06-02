#include "inputs.h"
#include "encoder.h"

#include <stdint.h>
#include <stdbool.h>

#include "gpio.h"
#include "rtt.h"

#include "appGPIO.h"
#include "debug_screen.h"
#include "race_screen.h"

encoder_t enc1 = {0};
encoder_t enc2 = {0};
encoder_t enc3 = {0};

void encoders_init(void)
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
}

void EXTI9_5_IRQHandler(void)
{
    // rprintf("EXTI9_5 IRQ\n");
    HAL_GPIO_EXTI_IRQHandler(EN2A_PIN);
    HAL_GPIO_EXTI_IRQHandler(EN2B_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == EN2A_PIN ||
        GPIO_Pin == EN2B_PIN)
    {
        encoder_interrupt(&enc2);
        // rprintf("IRQ %u pos=%u\n",enc2_irq_count, enc2.position);
    }
}
