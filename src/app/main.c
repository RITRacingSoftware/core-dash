#include <stdbool.h>
#include <stdio.h>

#include "can.h"
#include "clock.h"
#include "gpio.h"
#include "error_handler.h"
#include "boot.h"
#include "core_config.h"
#include "rtt.h"
#include "peripherals.h"
#include "encoder.h"
#include "appGPIO.h"
#include "appCAN.h"
#include "dash.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <stm32g4xx_hal.h>

#define TASK_PERIOD_HEARTBEAT_MS  200
#define TASK_PERIOD_DISPLAY_MS    200
#define TASK_PERIOD_INPUTS_MS     10
#define TASK_PERIOD_SOC_BAR_MS    1000

#define TASK_PRIORITY_CAN_RX   (tskIDLE_PRIORITY + 2)
#define TASK_PRIORITY_CAN_TX   (tskIDLE_PRIORITY + 2)
#define TASK_PRIORITY_DISPLAY  (tskIDLE_PRIORITY + 1)
#define TASK_PRIORITY_INPUTS   (tskIDLE_PRIORITY + 1)
#define TASK_PRIORITY_SOC_BAR  (tskIDLE_PRIORITY + 1)

static void hardfault_error_handler();
static void stack_overflow_error_handler();


void heartbeat_task(void *pvParameters) // 5 Hz
{
    (void) pvParameters;
    TickType_t next_wake_time = xTaskGetTickCount();
    while(true) 
    {
        core_GPIO_toggle_heartbeat();
        vTaskDelayUntil(&next_wake_time, TASK_PERIOD_HEARTBEAT_MS);
        // vTaskDelay(100 * portTICK_PERIOD_MS);
    }
}


void task_display(void *pvParameters) // 5 Hz
{
    (void) pvParameters;
    TickType_t next_wake_time = xTaskGetTickCount();
    while (true) 
    {
        if (!dash_task()) hardfault_error_handler(); 
        vTaskDelayUntil(&next_wake_time, TASK_PERIOD_DISPLAY_MS);
    }
}


void task_inputs(void *pvParameters) // 100 Hz
{
    (void) pvParameters;
    TickType_t next_wake_time = xTaskGetTickCount();

    while (true)
    {
        if (!inputs_task()) hardfault_error_handler(); 
        vTaskDelayUntil(&next_wake_time, TASK_PERIOD_INPUTS_MS);
    }
}


void task_soc_bar(void *pvParameters) // 1 Hz
{
    (void) pvParameters;
    TickType_t next_wake_time = xTaskGetTickCount();

    while (true)
    {
        if (!soc_bar_task()) hardfault_error_handler();
        vTaskDelayUntil(&next_wake_time, TASK_PERIOD_SOC_BAR_MS);
    }
}


void task_CAN_rx(void *pvParameters) 
{
    // rprintf("BEGIN CAN RX TASK\n");
    (void) pvParameters;
    while(true) CAN_rx();
}


void task_CAN_tx(void *pvParameters)
{
    (void) pvParameters;
    while(true) CAN_tx();
}


int main(void) {

    HAL_Init();
    if (!dash_init()) error_handler(); 

    int err;

    err = xTaskCreate(heartbeat_task, "heartbeat", 1000, NULL, 4, NULL);
    if (err != pdPASS) error_handler();

    err = xTaskCreate(task_display, "5Hz", 2000, NULL, TASK_PRIORITY_DISPLAY, NULL);
    if (err != pdPASS) hardfault_error_handler();

    err = xTaskCreate(task_inputs, "100Hz", 2000, NULL, TASK_PRIORITY_INPUTS, NULL);
    if (err != pdPASS) hardfault_error_handler();

    err = xTaskCreate(task_soc_bar, "1Hz", 2000, NULL, TASK_PRIORITY_SOC_BAR, NULL);
    if (err != pdPASS) hardfault_error_handler();

    err = xTaskCreate(task_CAN_rx, "CAN RX", 1000, NULL, TASK_PRIORITY_CAN_RX, NULL);
    if (err != pdPASS) hardfault_error_handler();

    err = xTaskCreate(task_CAN_tx, "CAN TX", 1000, NULL, TASK_PRIORITY_CAN_TX, NULL);
    if (err != pdPASS) hardfault_error_handler();
    
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    // hand control over to FreeRTOS
    vTaskStartScheduler();

    // we should not get here ever
    hardfault_error_handler();
    return 1;

}


void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) 
{
    (void) xTask;
    (void) pcTaskName;

    stack_overflow_error_handler();
}


void hardfault_error_handler()
{    
    while(1)
    {
        // rprintf("Error handler\n");
        core_GPIO_toggle_heartbeat();
        for (unsigned long long  i = 0; i < 200000; i++);
    }
}


void stack_overflow_error_handler()
{
    while(1)
    {
        core_GPIO_toggle_heartbeat();
        for (unsigned long long  i = 0; i < 400000; i++);
    }
}
