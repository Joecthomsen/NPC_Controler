// #ifndef STATE_MANAGER_H
// #define STATE_MANAGER_H

#pragma once

#include "driver/gpio.h"

#define STATE_MANAGER_GPIO_PIN_BLUE GPIO_NUM_4
#define STATE_MANAGER_GPIO_PIN_RED GPIO_NUM_5
#define STATE_MANAGER_GPIO_PIN_YELLOW GPIO_NUM_6
#define STATE_MANAGER_GPIO_PIN_GREEN GPIO_NUM_7

#define STATE_MANAGER_INTERRUPT_TYPE GPIO_INTR_DISABLE
#define STATE_MANAGER_PIN_DIRECTION GPIO_MODE_OUTPUT
#define STATE_MANAGER_PIN_PULLUP GPIO_PULLUP_DISABLE
#define STATE_MANAGER_PIN_PULLDOWN GPIO_PULLDOWN_DISABLE
#define LOW 0
#define HIGH 1

typedef enum State_t
{
    NVS_INIT_STATE,
    INIT_POP_STATE,
    AWAIT_WIFI_PROVISIONING_STATE,
    WIFI_PROVISIONING_STATE,
    WIFI_CONNECTED_STATE,
    DALI_COMMUNICATION_INIT_STATE,
    DALI_COMMUNICATION_OK_STATE,
    DALI_COMMISION_BUS_STATE,
    SYNCRONIZE_NVS_STATE,
    BLINK_LAMP_STATE,
    ANALYZE_DALI_BUS_STATE,
    TCP_SERVER_INIT_STATE,
    MDNS_INIT_STATE,
    AUTHENTICATION_STATE,
    NOT_AUTHENTICATED_STATE,
    SYSTEM_RUNNING_STATE,

    // Error states
    NO_WIFI_STATE,
    DALI_BUS_CORRUPTED_STATE,
    DALI_BUS_NOT_COMMISIONED_STATE,
    NO_RESPONSE_ON_DALI_BUS,

} State_t;

void set_state(int state);
State_t get_state();
void init_state_manager();
void state_task(void *parameter);
State_t get_last_state();

// #endif /* STATE_MANAGER_H */