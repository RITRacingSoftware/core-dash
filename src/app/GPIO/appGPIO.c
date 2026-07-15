#include "appGPIO.h"
//	#include "config.h"
#include "rtt.h"
#include "gpio.h"
#include "stm32g4xx_hal.h"

void GPIO_init()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();

	core_GPIO_init(EN1A_PORT, EN1A_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);
	core_GPIO_init(EN1B_PORT, EN1B_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);
	core_GPIO_init(PBEN1_PORT, PBEN1_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = EN2A_PIN | EN2B_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // core_GPIO_init(EN2A_PORT, EN2A_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);
	// core_GPIO_init(EN2B_PORT, EN2B_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);
	core_GPIO_init(PBEN2_PORT, PBEN2_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);

	core_GPIO_init(EN3A_PORT, EN3A_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);
	core_GPIO_init(EN3B_PORT, EN3B_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);
	core_GPIO_init(PBEN3_PORT, PBEN3_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);

	core_GPIO_init(PB1_PORT, PB1_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);
	core_GPIO_init(PTT_PORT, PTT_PIN, GPIO_MODE_INPUT, GPIO_PULLUP);

	core_GPIO_init(LED1_PORT, LED1_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);
	core_GPIO_init(LED2_PORT, LED2_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);

	core_GPIO_digital_write(LED1_PORT, LED1_PIN, false);
	core_GPIO_digital_write(LED2_PORT, LED2_PIN, false);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

bool GPIO_get_right_paddle_state() {return core_GPIO_digital_read(PB1_PORT, PB1_PIN);}
bool GPIO_get_enc1_pb_state()  {return core_GPIO_digital_read(PBEN1_PORT, PBEN1_PIN);}

