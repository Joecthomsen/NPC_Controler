#pragma once

#include <stdint.h>

#define SYNCHRONIZER_SHORT_ADDRESS_KEY "sync_short_adrs"
#define SYNCHRONIZER_MANUFACTOR_ID_ADDRESS_KEY "sync_manu_key"
#define SYNCHRONIZER_LOCATION_ON_BUS_KEY "sync_loc_on_bus"
#define SYNCHRONIZER_CONTROL_GEARS_STORED "sync_gears_stored"

typedef struct Synchronizer
{
    uint8_t short_address;
    uint64_t manufactor_id;
    uint8_t location_on_bus;
} Synchronizer_t;

typedef uint32_t *syncronizer_ptr_t;

syncronizer_ptr_t controle_gears[64];

Synchronizer_t get_controle_gear_from_short_address(uint8_t short_address);
Synchronizer_t get_controle_gear_from_manufactor_id(uint64_t manufactor_id);
Synchronizer_t get_all_controle_gears();
