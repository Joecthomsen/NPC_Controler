#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "constants.h"

DALI_Status check_drivers_commissioned(uint8_t *short_addresses_on_bus_count, uint8_t short_addresses_on_bus[64], uint8_t *uncommissioned_devices_on_bus_count, address24_t uncommissioned_devices_on_bus_address[64]);
uint8_t commission_bus();

void set_all_devices_in_initialize_state();
bool verify_short_address(uint8_t shortAddress);
void set_search_address(address24_t address);
