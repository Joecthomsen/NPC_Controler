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

// Memory bank 206 addresses
#define MEMORY_BANK_206 206
#define LOCK_BYTE 0x02U
#define LIGHT_SOURCE_START_COUNTER_MSB 0x04U
#define LIGHT_SOURCE_START_COUNTER_2 0x05U
#define LIGHT_SOURCE_START_COUNTER_LSB 0x06U
#define LIGHT_SOURCE_START_COUNTER_MSB 0x07U
#define LIGHT_SOURCE_START_COUNTER_2 0x08U
#define LIGHT_SOURCE_START_COUNTER_LSB 0x09U
#define LIGHT_SOURCE_ON_TIME_RESETTABLE_MSB 0x0AU
#define LIGHT_SOURCE_ON_TIME_RESETTABLE_2 0x0BU
#define LIGHT_SOURCE_ON_TIME_RESETTABLE_3 0x0CU
#define LIGHT_SOURCE_ON_TIME_RESETTABLE_LSB 0x0DU
#define LIGHT_SOURCE_ON_TIME_MSB 0x0EU
#define LIGHT_SOURCE_ON_TIME_2 0x0FU
#define LIGHT_SOURCE_ON_TIME_3 0x10U
#define LIGHT_SOURCE_ON_TIME_LSB 0x11U
#define LIGHT_SOURCE_VOLTAGE_MSB 0x12U
#define LIGHT_SOURCE_VOLTAGE_LSB 0x13U
#define LIGHT_SOURCE_CURRENT_MSB 0x14U
#define LIGHT_SOURCE_CURRENT_LSB 0x15U
#define LIGHT_SOURCE_OVERALL_FALIURE_CONDITION 0x16U
#define LIGHT_SOURCE_OVERALL_FALIURE_CONDITION_COUNTER 0x17U
#define LIGHT_SOURCE_SHORT_CIRCUIT 0x18U
#define LIGHT_SOURCE_SHORT_CIRCUIT_COUNTER 0x19U
#define LIGHT_SOURCE_OPEN_CIRCUIT 0x1AU
#define LIGHT_SOURCE_OPEN_CIRCUIT_COUNTER 0x1BU
#define LIGHT_SOURCE_THERMAL_DERATING 0x1CU
#define LIGHT_SOURCE_THERMAL_DERATING_COUNTER 0x1DU
#define LIGHT_SOURCE_THERMAL_SHUTDOWN 0x1EU
#define LIGHT_SOURCE_THERMAL_SHUTDOWN_COUNTER 0x1FU
#define LIGHT_SOURCE_TEMPERATURE 0x20U

// Memory bank 207 addresses
#define MEMORY_BANK_207 207
#define LOCK_BYTE 0x02U
#define RATED_MEDIAN_USEFULL_LIFE_OF_LUMINARE 0x04U
#define INTERNAL_CONTROLE_GEAR_REFERENCE_TEMPERATURE 0x05U
#define RATED_MEDIAN_USEFULL_LIGHT_SOURCE_STARTS_MSB 0x06U
#define RATED_MEDIAN_USEFULL_LIGHT_SOURCE_STARTS_LSB 0x07

typedef uint32_t bit24_t;

typedef struct Controle_gear_values_t
{
    // Memory bank 205
    uint64_t manufacturer_id;
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
    // Memory bank 206
    bit24_t light_source_start_counter_resettable;
    bit24_t light_source_start_counter;
    uint32_t light_source_on_time_resettable;
    uint32_t light_source_on_time;
    uint16_t light_source_voltage;
    uint16_t light_source_current;
    uint8_t light_source_overall_faliure_condition;
    uint8_t light_source_overall_faliure_condition_counter;
    uint8_t light_source_short_circuit;
    uint8_t light_source_short_circuit_counter;
    uint8_t light_source_open_circuit;
    uint8_t light_source_open_circuit_counter;
    uint8_t light_source_thermal_derating;
    uint8_t light_source_thermal_derating_counter;
    uint8_t light_source_thermal_shutdown;
    uint8_t light_source_thermal_shutdown_counter;
    uint8_t light_source_temperature;
    // Memory bank 207
    uint8_t rated_median_usefull_life_of_luminare;
    uint8_t internal_controle_gear_reference_temperature;
    uint16_t rated_median_usefull_light_source_starts;

} Controle_gear_values_t;

Controle_gear_values_t fetch_controle_gear_data(uint8_t short_address);

void printObject(Controle_gear_values_t controle_gear);
