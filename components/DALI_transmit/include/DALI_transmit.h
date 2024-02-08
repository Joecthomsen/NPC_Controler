#pragma once

#include "driver/gpio.h"
#include "driver/gptimer_types.h"
#include "driver/gptimer.h"
//#include "driver/timer.h"

#define GPIO_PIN_TX GPIO_NUM_2
#define GPIO_PIN_RX GPIO_NUM_3
#define HIGH 1
#define LOW 0

#define TIMER_GROUP    TIMER_GROUP_0
#define TIMER_NUMBER   TIMER_0
#define TIMER_FREQUENZ 1000000
#define TIMER_START_VALUE 0
#define BAUD_RATE 2400
#define TIMER_INTERVAL_US 500
#define DALI_IDLE_VALUE 1
#define MANCHESTER_ENCODED_0 0b10   //This is inverted, as the entire signal is inverted
#define MANCHESTER_ENCODED_1 0b01   //This is inverted, as the entire signal is inverted
#define TIMER_RX_OFFSET (TIMER_FREQUENZ/BAUD_RATE)*0.25 //Offset in order to read the RX data at the around 75% of the encoding

enum State {
    START_BIT,
    DATA,
    STOP_BIT,
};

void init_DALI_transmit();
void sendDALI_TX(uint16_t cmd);

extern int incrementer;
extern int rx_data_buffer[8]; 

 

/*

 #pragma once

#include "driver/gpio.h"
#include "driver/gptimer_types.h"
#include "driver/gptimer.h"

//#include "driver/timer.h"

//General settings
#define BAUD_RATE 2400

//GPIO general configuration
#define HIGH 1
#define LOW 0
#define DALI_IDLE_VALUE 1

//GPIO Tx configuration
#define GPIO_PIN_TX (1ULL << GPIO_NUM_2)
#define TX_INTERRUPT_TYPE GPIO_INTR_DISABLE
#define TX_PIN_DIRECTION GPIO_MODE_OUTPUT
#define TX_PIN_PULLUP GPIO_PULLUP_DISABLE
#define TX_PIN_PULLDOWN GPIO_PULLDOWN_DISABLE

//GPIO Rx configuration
#define GPIO_PIN_RX (1ULL << GPIO_NUM_3)
#define RX_INTERRUPT_TYPE GPIO_INTR_ANYEDGE
#define RX_PIN_DIRECTION GPIO_MODE_INPUT
#define RX_PIN_PULLUP GPIO_PULLUP_ENABLE
#define RX_PIN_PULLDOWN GPIO_PULLDOWN_DISABLE

//Timer Tx configuration
#define CLOCK_SOURCE_TX GPTIMER_CLK_SRC_DEFAULT
#define COUNTER_DIRECTION_TX GPTIMER_COUNT_UP
#define TIMER_FREQUENZ_TX 1000000                      //1MHz
#define TIMER_INTERRUPT_PRIORITY_TX 1

//Timer Rx configuration
#define CLOCK_SOURCE_RX GPTIMER_CLK_SRC_DEFAULT
#define COUNTER_DIRECTION_RX GPTIMER_COUNT_UP
#define TIMER_FREQUENZ_RX 1000000                      //1MHz
#define TIMER_INTERRUPT_PRIORITY_RX 0

//Alarm Tx configuration
#define ALARM_COUNT_TX TIMER_FREQUENZ_TX/BAUD_RATE
#define ALARM_AUTO_RELOAD_TX true
#define ALARM_RELOAD_COUNT_TX 0

// //Alarm Rx configuration
// #define ALARM_COUNT_RX (TIMER_FREQUENZ/BAUD_RATE)*2
// #define ALARM_AUTO_RELOAD_RX true
// #define ALARM_RELOAD_COUNT_RX 0

//Manchester encoding
#define MANCHESTER_ENCODED_0 0b10   //This is inverted, as the entire signal is inverted
#define MANCHESTER_ENCODED_1 0b01   //This is inverted, as the entire signal is inverted

enum State {
    START_BIT,
    DATA,
    STOP_BIT,
};

void init_DALI_transmit();
void sendDALI_TX(uint16_t cmd);

extern int incrementer;
extern int rx_data_buffer[8]; 

 
 */