#pragma once

#include <stdint.h>

#define RESPONSE_BUFFER_SIZE 128

typedef struct Controle_gear_t
{
    uint8_t short_address;
    uint8_t manufactoring_id;
} Controle_gear_t;

void test_me();

// char *get_all_controle_gear_as_json_string(void);
// char *get_controler_state(void);
// void blink_control_gear(uint8_t short_address);
// void stop_blink_control_gear(uint8_t short_address);
