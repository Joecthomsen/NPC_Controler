#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"

#define DELAY_BETWEEN_COMMANDS 25 / portTICK_PERIOD_MS // 25 milliseconds

typedef uint32_t address24_t; // 24 bit address

bool areDriversOnBusCommisioned();
uint8_t commissionDALIBus();
