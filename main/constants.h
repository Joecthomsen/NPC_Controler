#pragma once

#include "freertos/FreeRTOS.h"

#define DELAY_BETWEEN_COMMANDS 25 / portTICK_PERIOD_MS // 25 milliseconds
#define DELAY_AWAIT_RESPONSE 50 / portTICK_PERIOD_MS   // 50 milliseconds
#define ONE_HOUR 3600000 / portTICK_PERIOD_MS          // 3600000 mill
#define TCP_EVENT_BIT BIT0                             // tcp event bit

// Types
typedef uint32_t address24_t; // 24 bit address

typedef enum DALI_Status
{
    DALI_OK = 0,
    DALI_ERR_NO_RESPONSE,
    DALI_ERR_NO_RESPONSE_ON_BUS,
    DALI_ERR_BUS_CORRUPTED,
    DALI_ERR_BUS_NOT_COMMISIONED,
} DALI_Status;

extern EventGroupHandle_t tcpEventGroup;

extern uint8_t devices_on_bus_count;
extern uint8_t short_addresses_on_bus[64];

extern bool error_message_send;