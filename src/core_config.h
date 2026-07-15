#ifndef CORE_CORE_CONFIG_H
#define CORE_CORE_CONFIG_H

#define PROGRAM_NAME_STRING "Template project"

/***************** CLOCK PARAMETERS ****************************/
/***************************************************************/
#define CORE_CLOCK_USE_HSE 0
#define CORE_CLOCK_HSE_FREQ 24000
#define CORE_CLOCK_SYSCLK_FREQ 160000
#define CORE_CLOCK_HSI_FREQ 16000
#define CORE_CLOCK_PLLP_DIV 12

/***************** ERROR HANDLER PARAMETERS ********************/
/***************************************************************/
#define CORE_ERROR_HANDLER_BLINK_DELAY 200000


/********************** CAN PARAMETERS *************************/
/***************************************************************/
// CAN bitrate in Hz
#define CORE_CAN_BITRATE 1000000

// Number of CAN messages that can be stored in the CAN FreeRTOS queue
#define CORE_CAN_QUEUE_LENGTH 32

// Timeout for waiting on RX queue
#define CORE_CAN_RX_TIMEOUT 100

#define CORE_CAN_USE_MSGBUF 1
#define CORE_CAN_TIMESTAMP 1
#define CORE_CAN_HW_TIMESTAMP 1
#define CORE_CAN_MSGBUF1_SIZE 1024
#define CORE_CAN_MSGBUF2_SIZE 0
#define CORE_CAN_MSGBUF3_SIZE 0
#define CORE_FDCAN1_MSGBUF 1
#define CORE_FDCAN2_MSGBUF 1
#define CORE_FDCAN3_MSGBUF 1
#define CORE_CAN_TIMER TIM2

// Ports and pins for CAN communication
#define CORE_FDCAN1_TX_PORT GPIOA
#define CORE_FDCAN1_TX_PIN  GPIO_PIN_12
#define CORE_FDCAN1_TX_AF   9
#define CORE_FDCAN1_RX_PORT GPIOA
#define CORE_FDCAN1_RX_PIN  GPIO_PIN_11
#define CORE_FDCAN1_RX_AF   9

#define CORE_FDCAN2_TX_PORT GPIOB
#define CORE_FDCAN2_TX_PIN  GPIO_PIN_13
#define CORE_FDCAN2_TX_AF   9
#define CORE_FDCAN2_RX_PORT GPIOB
#define CORE_FDCAN2_RX_PIN  GPIO_PIN_12
#define CORE_FDCAN2_RX_AF   9

#define CORE_FDCAN3_TX_PORT GPIOA
#define CORE_FDCAN3_TX_PIN  GPIO_PIN_15
#define CORE_FDCAN3_TX_AF   11
#define CORE_FDCAN3_RX_PORT GPIOB
#define CORE_FDCAN3_RX_PIN  GPIO_PIN_3
#define CORE_FDCAN3_RX_AF   11

// Filters
#define CORE_FDCAN1_MAX_STANDARD_FILTER_NUM 28
#define CORE_FDCAN1_MAX_EXTENDED_FILTER_NUM 8
#define CORE_FDCAN2_MAX_STANDARD_FILTER_NUM 28
#define CORE_FDCAN2_MAX_EXTENDED_FILTER_NUM 8
#define CORE_FDCAN3_MAX_STANDARD_FILTER_NUM 28
#define CORE_FDCAN3_MAX_EXTENDED_FILTER_NUM 8

// Auto-retransmission config
#define CORE_FDCAN1_AUTO_RETRANSMISSION 1
#define CORE_FDCAN2_AUTO_RETRANSMISSION 0
#define CORE_FDCAN3_AUTO_RETRANSMISSION 0

// CAN FD config
#define CORE_FDCAN1_USE_FD 1
#define CORE_FDCAN2_USE_FD 0
#define CORE_FDCAN3_USE_FD 0

#define CORE_CAN_BUS_OFF_BLOCK 0 

/********************* SPI PARAMETERS **************************/
/***************************************************************/
#define CORE_SPI1_SCK_PORT  GPIOB
#define CORE_SPI1_SCK_PIN   GPIO_PIN_3
#define CORE_SPI1_SCK_AF    5
#define CORE_SPI1_MISO_PORT GPIOB
#define CORE_SPI1_MISO_PIN  GPIO_PIN_4
#define CORE_SPI1_MISO_AF   5
#define CORE_SPI1_MOSI_PORT GPIOB
#define CORE_SPI1_MOSI_PIN  GPIO_PIN_5
#define CORE_SPI1_MOSI_AF   5
#define CORE_SPI1_DIVIDER   7
#define CORE_SPI1_DATA_SIZE 8
#define CORE_SPI1_MASTER    1

#define CORE_SPI2_SCK_PORT  GPIOB
#define CORE_SPI2_SCK_PIN   GPIO_PIN_13
#define CORE_SPI2_SCK_AF    5
#define CORE_SPI2_MISO_PORT GPIOB
#define CORE_SPI2_MISO_PIN  GPIO_PIN_14
#define CORE_SPI2_MISO_AF   5
#define CORE_SPI2_MOSI_PORT GPIOB
#define CORE_SPI2_MOSI_PIN  GPIO_PIN_15
#define CORE_SPI2_MOSI_AF   5
#define CORE_SPI2_DIVIDER   7
#define CORE_SPI2_DATA_SIZE 8
#define CORE_SPI2_MASTER    1

#define CORE_SPI3_SCK_PORT  GPIOC
#define CORE_SPI3_SCK_PIN   GPIO_PIN_10
#define CORE_SPI3_SCK_AF    6
#define CORE_SPI3_MISO_PORT GPIOC
#define CORE_SPI3_MISO_PIN  GPIO_PIN_11
#define CORE_SPI3_MISO_AF   6
#define CORE_SPI3_MOSI_PORT GPIOC
#define CORE_SPI3_MOSI_PIN  GPIO_PIN_12
#define CORE_SPI3_MOSI_AF   6
#define CORE_SPI3_DIVIDER   7
#define CORE_SPI3_DATA_SIZE 8
#define CORE_SPI3_MASTER    1

#define CORE_SPI4_SCK_PORT  GPIOE
#define CORE_SPI4_SCK_PIN   GPIO_PIN_12
#define CORE_SPI4_SCK_AF    5
#define CORE_SPI4_MISO_PORT GPIOE
#define CORE_SPI4_MISO_PIN  GPIO_PIN_13
#define CORE_SPI4_MISO_AF   5
#define CORE_SPI4_MOSI_PORT GPIOE
#define CORE_SPI4_MOSI_PIN  GPIO_PIN_14
#define CORE_SPI4_MOSI_AF   5
#define CORE_SPI4_DIVIDER   7
#define CORE_SPI4_DATA_SIZE 8
#define CORE_SPI4_MASTER    1


/******************** USART PARAMETERS *************************/
/***************************************************************/
#define CORE_USART_RXBUFLEN 512
#define CORE_USART_RX_TIMEOUT 64

#define CORE_USART_UPRINTF 0
#define CORE_USART_TXBUFLEN 512

#define CORE_USART1_TX_PORT GPIOC
#define CORE_USART1_TX_PIN  GPIO_PIN_4
#define CORE_USART1_TX_AF   7
#define CORE_USART1_RX_PORT GPIOC
#define CORE_USART1_RX_PIN  GPIO_PIN_5
#define CORE_USART1_RX_AF   7

#define CORE_USART2_TX_PORT GPIOA
#define CORE_USART2_TX_PIN  GPIO_PIN_2
#define CORE_USART2_TX_AF   7
#define CORE_USART2_RX_PORT GPIOA
#define CORE_USART2_RX_PIN  GPIO_PIN_3
#define CORE_USART2_RX_AF   7

#define CORE_USART3_TX_PORT GPIOB
#define CORE_USART3_TX_PIN  GPIO_PIN_10
#define CORE_USART3_TX_AF   7
#define CORE_USART3_RX_PORT GPIOB
#define CORE_USART3_RX_PIN  GPIO_PIN_11
#define CORE_USART3_RX_AF   7

#define CORE_UART4_TX_PORT  GPIOC
#define CORE_UART4_TX_PIN   GPIO_PIN_10
#define CORE_UART4_TX_AF    5
#define CORE_UART4_RX_PORT  GPIOC
#define CORE_UART4_RX_PIN   GPIO_PIN_11
#define CORE_UART4_RX_AF    5
#define CORE_UART4_IRQ_PRIO 5

#define CORE_UART5_TX_PORT  GPIOC
#define CORE_UART5_TX_PIN   GPIO_PIN_12
#define CORE_UART5_TX_AF    5
#define CORE_UART5_RX_PORT  GPIOD
#define CORE_UART5_RX_PIN   GPIO_PIN_2
#define CORE_UART5_RX_AF    5


/*********************** RTC PARAMETERS ************************/
/***************************************************************/
#define CORE_RTC_CENTURY 2000


/******************** BOOTLOADER PARAMETERS ********************/
/***************************************************************/
#define CORE_BOOT_FDCAN FDCAN1
#define CORE_BOOT_FDCAN_ID 0x00A
#define CORE_BOOT_FDCAN_MASTER_ID 0x08A
#define CORE_BOOT_FDCAN_BROADCAST_ID 0x7ff


/********************* TIMEOUT PARAMETERS **********************/
/***************************************************************/
#define CORE_TIMEOUT_NUM 0


/******************** TIMESTAMP PARAMETERS *********************/
/***************************************************************/
//#define CORE_TIMESTAMP_MSB TIM2

/********************* WATCHDOG PARAMETERS *********************/
/***************************************************************/

// Timeout formula: timeout = (APB_clock_period(ms)) * 4096 * (2^(CORE_WATCHDOG_PRESCALER)) * (CORE_WATCHDOG_RELOAD_VAL + 1) ms
// APB clock set to system clock = 160MHz by default. APB_clock_period = 6.25ns = 6.25*10^(-6) ms
// Downcounter is 7 bits. When the lower 6 bits go to 0 (timer = 0x40), the interrupt is triggered if it is set.
// When the counter goes to 0x3F, the system reset is triggered.


// /**
//  * @brief Timebase of prescaler. 3 bits (max 7)
//  */
// #define CORE_WATCHDOG_PRESCALER 6
//
// /**
//  * @brief Lower 6 bits to reload into the downcounter. 6 bits (max 63)
//  */
// #define CORE_WATCHDOG_RELOAD_VAL 63
//
// /**
//  * @brief Watchdog window. Must refresh when downcounter has counted lower than this number
//  * for the refresh to be valid. If not, it will be reset. Must be greater than 0x40 to matter. 7 bits (max 63)
//  */
// #define CORE_WATCHDOG_WINDOW_VAL 3

/**
 * @brief Prescaler for watchdog timer driven by LSI (32kHz).
 * Frequency = 32kHz/(2^(CORE_WATCHDOG_PRESCALER + 2)). 3 bits (max 7)
 */
#define CORE_WATCHDOG_PRESCALER         6
/**
 * @brief Value that will be loaded into the watchdog counter upon reload. 11 bits (max 2047)
 */
#define CORE_WATCHDOG_RELOAD_VAL           500
/**
 * @brief Watchdog window. Must refresh when downcounter has counted lower than this number
 * for the refresh to be valid. If not, it will be reset 11 bits (max 2047)
 */
#define CORE_WATCHDOG_WINDOW_VAL           250

#endif //CORE_CORE_CONFIG_H
