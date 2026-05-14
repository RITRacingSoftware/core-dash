#include "display.h"
#include "RA8875.h"
#include "spi.h"
#include "rtt.h"
#include <stdint.h>
#include "gpio.h"
#include "appGPIO.h"

void display_init(void)
{
    core_SPI_init(RA8875_SPI, RA8875_CS_PORT, RA8875_CS_PIN);
    RA8875_SPI->CR1 |= 3; // modify clock phase and polarity
    core_GPIO_init(RA8875_RESET_PORT, RA8875_RESET_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);

    if (!RA8875_begin(
        RA8875_SPI,
        RA8875_RESET_PORT,
        RA8875_RESET_PIN,
        NULL,
        0,
        RA8875_480x272,
        RA8875_16BPP))
    {
        rprintf("Failed to Initialize Display\n");
    }

    rprintf("Display on\n");
    RA8875_display_on(1);

    GPIOX(1);
    RA8875_pwm1_config(1, RA8875_PWM_CLK_DIV1024);
    RA8875_pwm1_out(255);
    RA8875_graphic_mode();
   
    uint8_t id = RA8875_read_reg(0x00);
    rprintf("RA8875 ID = 0x%02X\n", id);


    for(int i = 0; i < 2; i++)
    {
        rprintf("Red\n");
        RA8875_fill_screen(RA8875_RED);
        HAL_Delay(500);

        rprintf("Green\n");
        RA8875_fill_screen(RA8875_GREEN);
        HAL_Delay(500);

        rprintf("Blue\n");
        RA8875_fill_screen(RA8875_BLUE);
        HAL_Delay(500);
    }

    RA8875_fill_screen(RA8875_BLACK);
    RA8875_text_mode();
    RA8875_text_cursor_position(50, 50);
    RA8875_text_transparent_color(RA8875_WHITE);
    RA8875_text_scale(2);
    RA8875_text_write("Hello World", 0);
}
