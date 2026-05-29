#pragma once

#include <stdbool.h>

// ENCODER 1
#define EN1A_PORT  GPIOC
#define EN1A_PIN   GPIO_PIN_7

#define EN1B_PORT  GPIOC
#define EN1B_PIN   GPIO_PIN_8

#define PBEN1_PORT GPIOC
#define PBEN1_PIN  GPIO_PIN_9


// ENCODER 2
#define EN2A_PORT  GPIOA
#define EN2A_PIN   GPIO_PIN_9

#define EN2B_PORT  GPIOA
#define EN2B_PIN   GPIO_PIN_8

#define PBEN2_PORT GPIOA
#define PBEN2_PIN  GPIO_PIN_10


// ENCODER 3
#define EN3A_PORT  GPIOA
#define EN3A_PIN   GPIO_PIN_0

#define EN3B_PORT  GPIOC
#define EN3B_PIN   GPIO_PIN_3

#define PBEN3_PORT GPIOC
#define PBEN3_PIN  GPIO_PIN_1


// PUSHBUTTONS
#define PB1_PORT   GPIOC
#define PB1_PIN    GPIO_PIN_0

#define PTT_PORT   GPIOA
#define PTT_PIN    GPIO_PIN_1


// LEDS
#define LED1_PORT  GPIOB
#define LED1_PIN   GPIO_PIN_10

#define LED2_PORT  GPIOB
#define LED2_PIN   GPIO_PIN_2

void GPIO_init();
bool GPIO_get_button1_state();

