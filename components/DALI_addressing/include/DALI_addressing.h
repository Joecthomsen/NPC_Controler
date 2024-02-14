#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "constants.h"

DALI_Status check_drivers_commissioned();
uint8_t commission_bus();
