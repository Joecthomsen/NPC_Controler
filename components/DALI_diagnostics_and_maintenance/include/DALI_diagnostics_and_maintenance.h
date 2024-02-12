#include <stdint.h>

// Commands
#define DTR0 0xA300
#define DTR1 0xC300
#define READ_MEMORY_LOCATION 0x00C5 // 00 is the device.

// Memory bank 205 addresses
#define MEMORY_BANK_205 205
#define OPERATING_TIME_MSB 0x04
#define OPERATING_TIME_2 0x05
#define OPERATING_TIME_3 0x06
#define OPERATING_TIME_LSB 0x07

typedef uint32_t bit24_t;

typedef struct Controle_gear
{
    uint32_t operating_time;
    bit24_t start_counter;
    uint16_t external_supply_voltage;
    uint8_t external_supply_voltage_frequency;
    uint8_t power_factor;
    uint8_t overall_faliure_condition;
    uint8_t external_supply_undervoltage;
    uint8_t external_supply_undervoltage_counter;
    uint8_t external_supply_overvoltage;
    uint8_t external_supply_overvoltage_counter;
    uint8_t output_power_limitation;
    uint8_t output_power_limitation_counter;
    uint8_t thermal_derating;
    uint8_t thermal_derating_counter;
    uint8_t thermal_shutdown;
    uint8_t thermal_shutdown_counter;
    uint8_t temperature;
    uint8_t output_current_percent;
} Controle_gear;

Controle_gear fetch_controle_gear_data(uint8_t short_address);
