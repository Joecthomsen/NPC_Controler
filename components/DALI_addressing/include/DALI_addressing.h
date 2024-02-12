#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"

#define DELAY_BETWEEN_COMMANDS 25 / portTICK_PERIOD_MS // 25 milliseconds
#define DELAY_AWAIT_RESPONSE 50 / portTICK_PERIOD_MS   // 50 milliseconds

typedef uint32_t address24_t; // 24 bit address

typedef enum DALI_Status
{
    DALI_OK = 0,
    DALI_ERR_NO_RESPONSE,
    DALI_ERR_NO_DRIVERS,
    DALI_ERR_UNCOMMISSIONED_DRIVER,
    DALI_ERR_INVALID_SHORT_ADDR,
    DALI_ERR_INVALID_LONG_ADDR,
    DALI_ERR_INVALID_COMMAND,
    DALI_ERR_INVALID_RESPONSE,
    DALI_ERR_INVALID_DRIVER_COUNT,
    DALI_ERR_INVALID_DRIVER_INDEX,
    DALI_ERR_INVALID_DRIVER_STATE,
    DALI_ERR_INVALID_DRIVER_SHORT_ADDR,
    DALI_ERR_INVALID_DRIVER_LONG_ADDR,
    DALI_ERR_INVALID_DRIVER_TYPE
} DALI_Status;

DALI_Status check_drivers_commissioned();
uint8_t commission_bus();
