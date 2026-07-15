#include "neopixel.h"

#include <stdint.h>

#include "core.h"
#include "stm32g4xx_hal.h"

#define BAUD_RATE 2400000 // 2.4Mbps
#define PIXEL_UART USART1 // USART interface

#define NEOPIXEL_UART_BYTES_PER_PIXEL 8
#define NEOPIXEL_TX_LEN (NEOPIXEL_COUNT * NEOPIXEL_UART_BYTES_PER_PIXEL)                  


static const uint8_t bit_lut[8] = 
{
//   inverted and reversed for UART
//   start bit = 0 , stop bit = 1
//   input is 3 neopixel bits , output is 9 uart bits (1x 7-bit message)

//   1d01d01d0
    0b1011011, // 000 : 111 : 111
    0b0011011, // 001 : 110 : 011
    0b1010011, // 010 : 101 : 101
    0b0010011, // 011 : 100 : 001
    0b1011010, // 100 : 011 : 110
    0b0011010, // 101 : 010 : 010
    0b1010010, // 110 : 001 : 100
    0b0010010  // 111 : 000 : 000

};

// static uint8_t txbuf[NEOPIXEL_TX_LEN];
static neopixel_t pixels[NEOPIXEL_COUNT];


static void neopixel_reset_delay(void)
{
    uint64_t ts = core_timestamp_get_tick();  
    while ((core_timestamp_get_tick() - ts) < 500); // 500us delay   
}

void neopixel_set(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index >= NEOPIXEL_COUNT) return;

    pixels[index].r = r;
    pixels[index].g = g;
    pixels[index].b = b;
}

void neopixel_clear()
{
    for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++) 
    {
        neopixel_set(i, 0, 0, 0);
    }

    neopixel_write();
}

void neopixel_init(void)
{
    core_USART_init(PIXEL_UART, BAUD_RATE);

    USART1->CR1 &= ~USART_CR1_UE;   // disable UART
    USART1->CR1 |= USART_CR1_M1;    // 7-bit mode
    USART1->CR2 |= USART_CR2_TXINV; // UART inversion
    USART1->CR1 |= USART_CR1_UE;    // enable
    
    neopixel_reset_delay();
    neopixel_clear();
    neopixel_write();
}

void neopixel_write()
{

    // uint16_t tx_index = 0;
    __disable_irq();

    for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++)
    {
        uint8_t neopixel_data[3] = {pixels[i].g, pixels[i].r, pixels[i].b};
        uint16_t sr = ((uint16_t)neopixel_data[0] << 8) | neopixel_data[1];
   
        uint8_t fill = 0;
        uint8_t j = 2;

        while (j < 3)
        {
            uint8_t uart_byte = bit_lut[sr>>13];
            core_USART_transmit(PIXEL_UART, &uart_byte, 1);
            // txbuf[tx_index++] = bit_lut[sr>>13];

            sr = sr << 3;
            fill += 3;

            if (fill >= 8) 
            {
                fill -= 8;
                sr |= ((uint16_t)neopixel_data[j++] << fill);
            }
        }

        while (fill < 16)
        {
            uint8_t uart_byte = bit_lut[sr>>13];
            core_USART_transmit(PIXEL_UART, &uart_byte, 1);
            // txbuf[tx_index++] = bit_lut[sr>>13];

            sr = sr << 3;
            fill += 3;
        }
    }

    // core_USART_transmit(PIXEL_UART, txbuf, tx_index);
    neopixel_reset_delay();
    __enable_irq();
}

void neopixel_delay_ms(uint32_t delay_ms)
{
    uint64_t ts = core_timestamp_get_tick();

    while ((core_timestamp_get_tick() - ts) < (delay_ms * 1000));
}
