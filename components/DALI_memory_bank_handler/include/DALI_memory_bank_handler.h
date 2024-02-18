#pragma once

#include "constants.h"

// Commands
#define DTR0 0xA300
#define DTR1 0xC300
#define READ_MEMORY_LOCATION 0x00C5U // 00 is the device.
#define WRITE_MEMORY_LOCATION 0xC700U

DALI_Status read_memory_location(uint8_t short_address, uint8_t memory_bank, uint8_t location, uint8_t *data);
DALI_Status write_memory_location(uint8_t short_address, uint8_t memory_bank, uint8_t location, uint8_t data);
uint64_t read_manufactor_id(uint8_t short_address);
