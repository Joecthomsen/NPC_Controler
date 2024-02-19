#pragma once

#include <stdint.h>

#define SYNCHRONIZER_SHORT_ADDRESS_KEY "sync_short_adrs"
#define SYNCHRONIZER_MANUFACTOR_ID_ADDRESS_KEY "sync_manu_key"

typedef struct Synchronizer
{
    uint8_t short_address;
    uint64_t manufactor_id;
} Synchronizer_t;

typedef uint32_t syncronizer_struct_t;
