#pragma once

#include "esp_err.h"
#include <stdbool.h>

#define NVS_DRIVERS_STORED "nvs_drivers"
#define NVS_ARE_BUS_COMMISIONED "nvs_are_bus_commisioned"

void init_nvs_handler(void);
void nvs_write_uint8(const char *key, uint8_t value);
uint8_t nvs_read_uint8(const char *key);
// bool nvs_value_exists(const char *key);