#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"

#define DELAY_BETWEEN_COMMANDS 25 // 15 milliseconds

typedef uint32_t address24_t; // 24 bit address

bool areDriversOnBusCommisioned();
void commissionDALIBus(uint8_t *addressArray);

typedef struct Driver
{
    uint8_t shortAddress;
    uint8_t location;
} Driver;