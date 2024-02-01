
//#include <cstring>
#include "driver/gpio.h"
#include "driver/gptimer_types.h"
#include "driver/gptimer.h"
//#include "driver/timer.h"

#define GPIO_PIN GPIO_NUM_2

#define TIMER_GROUP    TIMER_GROUP_0
#define TIMER_NUMBER   TIMER_0
#define TIMER_FREQUENZ 1000000
#define TIMER_START_VALUE 0
#define BAUD_RATE 2400
#define TIMER_INTERVAL_US 500
#define DALI_IDLE_VALUE 0
#define MANCHESTER_ENCODED_0 0b10   //This is inverted, as the entire signal is inverted
#define MANCHESTER_ENCODED_1 0b01   //This is inverted, as the entire signal is inverted

enum State {
    START_BIT,
    DATA,
    STOP_BIT,
};

void init_DALI_transmit();
void sendDALI_TX(uint16_t cmd);
uint32_t manchesterEncode(uint16_t data);
uint8_t percentToDalimapping(uint8_t input);

extern int incrementer;;

 