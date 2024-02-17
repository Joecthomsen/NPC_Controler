#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "driver/gpio.h"

#define STATE_MANAGER_GPIO_PIN_BLUE GPIO_NUM_4
#define STATE_MANAGER_GPIO_PIN_RED GPIO_NUM_5

#define STATE_MANAGER_INTERRUPT_TYPE GPIO_INTR_DISABLE
#define STATE_MANAGER_PIN_DIRECTION GPIO_MODE_OUTPUT
#define STATE_MANAGER_PIN_PULLUP GPIO_PULLUP_DISABLE
#define STATE_MANAGER_PIN_PULLDOWN GPIO_PULLDOWN_DISABLE
#define LOW 0
#define HIGH 1

typedef enum State_Machine
{
    STATE_NOT_WIFI_PROVISIONED,
    STATE_INIT_DALI_COM,
    STATE_RUNNING,
    STATE_ERROR,
    STATE_BUS_ERROR,
    STATE_BUS_NOT_COMMISSIONED,
} State_Machine;

void set_state(State_Machine state);
State_Machine get_state();
void init_state_manager();
void state_task(void *parameter);

#endif /* STATE_MANAGER_H */