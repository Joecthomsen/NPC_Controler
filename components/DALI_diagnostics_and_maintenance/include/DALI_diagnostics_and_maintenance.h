#include <stdint.h>

// Memory bank 205 addresses
#define MEMORY_BANK_205 205
#define LOCK_BYTE 0x02U
#define OPERATING_TIME_MSB 0x04U
#define OPERATING_TIME_2 0x05U
#define OPERATING_TIME_3 0x06U
#define OPERATING_TIME_LSB 0x07U
#define START_COUNTER_MSB 0x08U
#define START_COUNTER_2 0x09U
#define START_COUNTER_LSB 0x0AU
#define EXTERNAL_SUPPLY_VOLTAGE_MSB 0x0BU
#define EXTERNAL_SUPPLY_VOLTAGE_LSB 0x0CU
#define EXTERNAL_SUPPLY_VOLTAGE_FREQUENZY 0x0DU
#define POWER_FACTOR 0x0EU
#define OVERALL_FALIURE_CONDITION 0x0FU
#define EXTERNAL_SUPPLY_UNDERVOLTAGE 0x11U
#define EXTERNAL_SUPPLY_UNDERVOLTAGE_COUNTER 0x12U
#define EXTERNAL_SUPPLY_OVERVOLTAGE 0x13U
#define EXTERNAL_SUPPLY_OVERVOLTAGE_COUNTER 0x14U
#define OUTPUT_POWER_LIMITATION 0x15U
#define OUTPUT_POWER_LIMITATION_COUNTER 0x16U
#define THERMAL_DERATING 0x17U
#define THERMAL_DERATING_COUNTER 0x18U
#define THERMAL_SHUTDOWN 0x19U
#define THERMAL_SHUTDOWN_COUNTER 0x1AU
#define TEMPERATURE 0x1BU
#define OUTPUT_CURRENT_PERCENT 0x1CU

typedef uint32_t bit24_t;

typedef struct Controle_gear_values_t
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
} Controle_gear_values_t;

Controle_gear_values_t fetch_controle_gear_data(uint8_t short_address);

void printObject(Controle_gear_values_t controle_gear);
