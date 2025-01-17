#pragma once

#include "driver/gpio.h"

// GPIO Rx configuration
#define GPIO_PIN_INPUT_BUTTON GPIO_NUM_1
#define INPUT_BUTTON_INTERRUPT_TYPE GPIO_INTR_ANYEDGE
#define INPUT_BUTTON_PIN_DIRECTION GPIO_MODE_INPUT
#define INPUT_BUTTON_PIN_PULLUP GPIO_PULLUP_ENABLE
#define INPUT_BUTTON_PIN_PULLDOWN GPIO_PULLDOWN_DISABLE

#define BUTTON_HOLD_DURATION pdMS_TO_TICKS(10000) // 10 seconds in ticks

void init_input_button();