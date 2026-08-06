#include "peripherals.h"
#include "encoder.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gpio.h"
#include "rtt.h"

#include "appGPIO.h"
#include "neopixel.h"
#include "debug_screen.h"
#include "race_screen.h"
#include "DataManager.h"

//#define MAX_DELTA 65535
#define MAX_DELTA     5000
#define LED_CENTER_INDEX 5
#define SIDE_LED_COUNT   5


//---------------- ENCODERS ----------------//

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
        if (dash_data.vc_status.vc_status_vehicle_state != READY_TO_DRIVE_STATE) encoder_interrupt(&enc2);
        // rprintf("IRQ %u pos=%u\n",enc2_irq_count, enc2.position);
    }
}


//---------------- NEOPIXELS ----------------//

static uint8_t calculate_led_count(int32_t delta)
{
    uint32_t mag = abs(delta);

    uint8_t count = (mag * SIDE_LED_COUNT) / MAX_DELTA;

    if (count > SIDE_LED_COUNT) count = SIDE_LED_COUNT;

    return count;
}

static void endurance_set_color(uint8_t index)
{
    if (index <= 4) neopixel_set(index, 255, 0, 0);            // RED     (1-4)
    else if (index == 5) neopixel_set(index, 255, 255, 0);     // YELLOW  (5-7)
    else neopixel_set(index, 0, 255, 0);                       // GREEN   (8-11)
}

void update_endurance_neopixels (int32_t delta)
{
    uint8_t led_count = calculate_led_count(delta);
    // rprintf("delta = %u\n", delta);
    // rprintf("led count = %u\n", led_count);

    neopixel_clear();

    endurance_set_color(LED_CENTER_INDEX);

    if (delta < 0) // update left LEDs
    {
        for (uint8_t i = 1; i <= led_count; i++)
        {
            endurance_set_color(LED_CENTER_INDEX - i);
        }
    }

    else if (delta > 0) // update right LEDs
    {
        for (uint8_t i = 1; i <= led_count; i++)
        {
            endurance_set_color(LED_CENTER_INDEX + i);
        }
    }

    neopixel_write();
}

void soc_startup(void)
{
    neopixel_clear();

    // 1) Center pulse outward
    for (uint8_t i = 0; i <= SIDE_LED_COUNT; i++)
    {
        neopixel_clear();

        endurance_set_color(LED_CENTER_INDEX);

        if (i > 0)
        {
            endurance_set_color(LED_CENTER_INDEX - i);
            endurance_set_color(LED_CENTER_INDEX + i);
        }

        neopixel_write();
        neopixel_delay_ms(80);
    }

    // 2) Fast red-to-green scanner (1x pass)
    for (uint8_t pass = 0; pass < 1; pass++)
    {
        for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++)
        {
            neopixel_clear();

            endurance_set_color(i);

            if (i > 0) endurance_set_color(i - 1);
            if (i + 1 < NEOPIXEL_COUNT) endurance_set_color(i + 1);

            neopixel_write();
            neopixel_delay_ms(45);
        }

        for (int8_t i = NEOPIXEL_COUNT - 2; i > 0; i--)
        {
            neopixel_clear();

            endurance_set_color(i);

            if (i > 0) endurance_set_color(i - 1);
            if (i + 1 < NEOPIXEL_COUNT) endurance_set_color(i + 1);

            neopixel_write();
            neopixel_delay_ms(45);
        }
    }

    // 3) Fill everything from center outward
    neopixel_clear();
    endurance_set_color(LED_CENTER_INDEX);
    neopixel_write();
    neopixel_delay_ms(80);

    for (uint8_t i = 1; i <= SIDE_LED_COUNT; i++)
    {
        endurance_set_color(LED_CENTER_INDEX - i);
        endurance_set_color(LED_CENTER_INDEX + i);

        neopixel_write();
        neopixel_delay_ms(70);
    }

    // 4) Collapse back to center
    for (uint8_t i = SIDE_LED_COUNT; i > 0; i--)
    {
        neopixel_set(LED_CENTER_INDEX - i, 0, 0, 0);
        neopixel_set(LED_CENTER_INDEX + i, 0, 0, 0);

        neopixel_write();
        neopixel_delay_ms(70);
    }

    neopixel_clear();

}


