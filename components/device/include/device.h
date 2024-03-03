#pragma once

#include <stdint.h>

typedef struct Device_t
{
    uint8_t short_address;
    uint64_t manufactoring_id;
    char refresh_token[512];
} Device_t;
