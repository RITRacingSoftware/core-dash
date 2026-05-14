#pragma once

#define RA8875_SPI          SPI1
#define RA8875_CS_PORT      GPIOA
#define RA8875_CS_PIN       GPIO_PIN_15
#define RA8875_RESET_PORT   GPIOB
#define RA8875_RESET_PIN    GPIO_PIN_6

void display_init(void);
