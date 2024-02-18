#pragma once

#include "freertos/FreeRTOS.h"

#define DELAY_BETWEEN_COMMANDS 25 / portTICK_PERIOD_MS // 25 milliseconds
#define DELAY_AWAIT_RESPONSE 50 / portTICK_PERIOD_MS   // 50 milliseconds

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
