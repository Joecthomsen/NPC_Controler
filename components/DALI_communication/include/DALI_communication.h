#pragma once

#include "driver/gpio.h"

// Commands
#define INITIALIZE_ALL_DEVICE 0xA500   // Send twice
#define GENERATE_RANDOM_ADDRESS 0xA700 // Send twice
#define COMPARE 0xA900
#define WITHDRAW 0xAB00
#define SEARCH_ADDRESS_H 0xB100
#define SEARCH_ADDRESS_M 0xB300
#define SEARCH_ADDRESS_L 0xB500
#define PROGRAM_SHORT_ADDRESS 0xB701 // plus address
#define QUERY_SHORT_ADDRESS 0xBB01   // plus address
#define VERIFY_SHORT_ADDRESS 0xB901  // plus address
#define TERMINATE 0xA100
#define BROADCAST_DAPC_ON 0xFEFE
#define BROADCAST_DAPC_OFF 0xFE00
// #define DAPC_ON (short_address)(short_address << 1) + 1

// General settings
#define BAUD_RATE 2400 // Used for transmitting baudrate
#define DALI_IDLE_VALUE 1
#define HIGH 1
#define LOW 0

// GPIO Tx configuration
#define GPIO_PIN_TX GPIO_NUM_2
#define TX_INTERRUPT_TYPE GPIO_INTR_DISABLE
#define TX_PIN_DIRECTION GPIO_MODE_OUTPUT
#define TX_PIN_PULLUP GPIO_PULLUP_DISABLE
#define TX_PIN_PULLDOWN GPIO_PULLDOWN_DISABLE

// GPIO Rx configuration
#define GPIO_PIN_RX GPIO_NUM_3
#define RX_INTERRUPT_TYPE GPIO_INTR_ANYEDGE
#define RX_PIN_DIRECTION GPIO_MODE_INPUT
#define RX_PIN_PULLUP GPIO_PULLUP_ENABLE
#define RX_PIN_PULLDOWN GPIO_PULLDOWN_DISABLE

// Timer Tx configuration
#define CLOCK_SOURCE_TX GPTIMER_CLK_SRC_DEFAULT
#define COUNTER_DIRECTION_TX GPTIMER_COUNT_UP
#define TIMER_FREQUENZ_TX 1000000 // 1MHz
#define TIMER_INTERRUPT_PRIORITY_TX 1

// Timer Rx configuration
#define CLOCK_SOURCE_RX GPTIMER_CLK_SRC_DEFAULT
#define COUNTER_DIRECTION_RX GPTIMER_COUNT_UP
#define TIMER_FREQUENZ_RX 1000000 // 1MHz
#define TIMER_INTERRUPT_PRIORITY_RX 2

// Alarm Tx configuration
#define ALARM_COUNT_TX TIMER_FREQUENZ_TX / BAUD_RATE
#define ALARM_AUTO_RELOAD_TX true
#define ALARM_RELOAD_COUNT_TX 0

// Manchester encoding
#define MANCHESTER_ENCODED_0 0b10
#define MANCHESTER_ENCODED_1 0b01

enum State
{
    START_BIT,
    DATA,
    STOP_BIT,
};

void init_DALI_communication();
void send_DALI_Tx(uint16_t cmd);
bool new_data_available();
void clear_new_data_flag();
uint8_t get_new_data();

extern int incrementer;
extern int incrementer2;
extern int rx_data_buffer[8];