#include <stdio.h>
#include "DALI_diagnostics_and_maintenance.h"
#include "DALI_communication.h"
#include "freertos/FreeRTOS.h"
#include "constants.h"
#include "Memory_bank_handler.h"

// Prototypes
// DALI_Status read_memory_location(uint8_t short_address, uint8_t memory_bank, uint8_t location, uint8_t *data);
// DALI_Status write_memory_location(uint8_t short_address, uint8_t memory_bank, uint8_t location, uint8_t data);
// void select_memory_bank_location(uint8_t memory_bank, uint8_t location);

DALI_Status get_operating_time(uint8_t short_address, uint32_t *operating_time);
DALI_Status get_start_counter(uint8_t short_address, bit24_t *start_counter);
DALI_Status get_external_supply_voltage(uint8_t short_address, uint16_t *external_supply_voltage);
uint8_t calculate_short_address_standard_cmd(uint8_t short_address);
uint64_t read_manufactor_id(uint8_t short_address);

/**
 * @brief Fetch diagnostics data from a DALI control gear
 *
 * @param short_address Control gear short address
 * @return Controle_gear Struct containing diagnostics data
 */
Controle_gear fetch_controle_gear_data(uint8_t short_address)
{
    Controle_gear controle_gear;
    DALI_Status dali_status;

    printf("Manufactor ID: %llx\n", read_manufactor_id(short_address));

    uint32_t new_operating_time = 0;
    bit24_t new_start_counter = 0;
    uint16_t new_external_supply_voltage = 0;
    uint8_t new_external_supply_voltage_frequenzy = 0;
    uint8_t new_power_factor;
    uint8_t new_overall_faliure_condition;
    uint8_t new_external_supply_undervoltage;
    uint8_t new_external_supply_undervoltage_counter;
    uint8_t new_external_supply_overvoltage;
    uint8_t new_external_supply_overvoltage_counter;
    uint8_t new_output_power_limitation;
    uint8_t new_output_power_limitation_counter;
    uint8_t new_thermal_derating;
    uint8_t new_thermal_derating_counter;
    uint8_t new_thermal_shutdown;
    uint8_t new_thermal_shutdown_counter;
    uint8_t new_temperature;
    uint8_t new_output_current_percent;

    send_DALI_Tx(0x0181);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    send_DALI_Tx(0x0181);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);

    write_memory_location(short_address, MEMORY_BANK_205, LOCK_BYTE, 0x55);

    dali_status = get_operating_time(short_address, &new_operating_time);
    if (dali_status != DALI_OK)
        printf("Operation time error: %d\n", dali_status);

    dali_status = get_start_counter(short_address, &new_start_counter);
    if (dali_status != DALI_OK)
        printf("Start counter error: %d\n", dali_status);

    dali_status = get_external_supply_voltage(short_address, &new_external_supply_voltage);
    if (dali_status != DALI_OK)
        printf("External supply voltage error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_VOLTAGE_FREQUENZY, &new_external_supply_voltage_frequenzy);
    if (dali_status != DALI_OK)
        printf("External supply voltage frequenzy error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, POWER_FACTOR, &new_power_factor);
    if (dali_status != DALI_OK)
        printf("Power factor error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OVERALL_FALIURE_CONDITION, &new_overall_faliure_condition);
    if (dali_status != DALI_OK)
        printf("Overall faliure condition: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_UNDERVOLTAGE, &new_external_supply_undervoltage);
    if (dali_status != DALI_OK)
        printf("External supply undervoltage counter error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_UNDERVOLTAGE_COUNTER, &new_external_supply_undervoltage_counter);
    if (dali_status != DALI_OK)
        printf("External supply undervoltage counter error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_OVERVOLTAGE, &new_external_supply_overvoltage);
    if (dali_status != DALI_OK)
        printf("External supply undervoltage counter error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_OVERVOLTAGE_COUNTER, &new_external_supply_overvoltage_counter);
    if (dali_status != DALI_OK)
        printf("External supply undervoltage counter error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OUTPUT_POWER_LIMITATION, &new_output_power_limitation);
    if (dali_status != DALI_OK)
        printf("Power limitation error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OUTPUT_POWER_LIMITATION_COUNTER, &new_output_power_limitation_counter);
    if (dali_status != DALI_OK)
        printf("Power limitation counter error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, THERMAL_DERATING, &new_thermal_derating);
    if (dali_status != DALI_OK)
        printf("Therman derating error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, THERMAL_DERATING_COUNTER, &new_thermal_derating_counter);
    if (dali_status != DALI_OK)
        printf("Therman derating counter error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, THERMAL_SHUTDOWN, &new_thermal_shutdown);
    if (dali_status != DALI_OK)
        printf("Therman derating counter error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, THERMAL_SHUTDOWN_COUNTER, &new_thermal_shutdown_counter);
    if (dali_status != DALI_OK)
        printf("Therman derating counter error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, TEMPERATURE, &new_temperature);
    if (dali_status != DALI_OK)
        printf("Therman derating counter error: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OUTPUT_CURRENT_PERCENT, &new_output_current_percent);
    if (dali_status != DALI_OK)
        printf("Therman derating counter error: %d\n", dali_status);

    uint8_t locableByte = 0;
    dali_status = read_memory_location(short_address, MEMORY_BANK_205, 0x02, &locableByte);
    if (dali_status != DALI_OK)
        printf("Locable byte error: %d\n", dali_status);

    printf("locableByte: %d\n", locableByte);

    controle_gear.operating_time = new_operating_time;
    controle_gear.start_counter = new_start_counter;
    controle_gear.external_supply_voltage = new_external_supply_voltage;
    controle_gear.external_supply_voltage_frequency = new_external_supply_voltage_frequenzy;
    controle_gear.power_factor = new_power_factor;
    controle_gear.overall_faliure_condition = new_overall_faliure_condition;
    controle_gear.external_supply_undervoltage = new_external_supply_undervoltage;
    controle_gear.external_supply_undervoltage_counter = new_external_supply_undervoltage_counter;
    controle_gear.external_supply_overvoltage = new_external_supply_overvoltage;
    controle_gear.external_supply_overvoltage_counter = new_external_supply_overvoltage_counter;
    controle_gear.output_power_limitation = new_output_power_limitation;
    controle_gear.output_power_limitation_counter = new_output_power_limitation_counter;
    controle_gear.thermal_derating = new_thermal_derating;
    controle_gear.thermal_derating_counter = new_thermal_derating_counter;
    controle_gear.thermal_shutdown = new_thermal_shutdown;
    controle_gear.thermal_shutdown_counter = new_thermal_shutdown_counter;
    controle_gear.temperature = new_temperature;
    controle_gear.output_current_percent = new_output_current_percent;

    return controle_gear;
}

/**
 * @brief Get operating hours from DALI device
 *
 * @param short_address DALI device short address
 * @param operating_time Pointer to uint32_t to store operating hours
 * @return DALI_Status DALI_OK if successful
 */
DALI_Status get_operating_time(uint8_t short_address, uint32_t *operating_time)
{
    uint8_t operating_time_MSB = 0;
    uint8_t operating_time_2 = 0;
    uint8_t operating_time_3 = 0;
    uint8_t operating_time_LSB = 0;

    DALI_Status dali_status;

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OPERATING_TIME_MSB, &operating_time_MSB);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OPERATING_TIME_2, &operating_time_2);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OPERATING_TIME_3, &operating_time_3);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OPERATING_TIME_LSB, &operating_time_LSB);
    if (dali_status != DALI_OK)
        return dali_status;

    *operating_time |= ((operating_time_MSB << 0x18U) | (operating_time_2 << 0x10U) | (operating_time_3 << 0x8U) | operating_time_LSB);

    return dali_status;
}

/**
 * @brief Get start counter from DALI device
 *
 * @param short_address DALI device short address
 * @param start_counter Pointer to 24-bit value to store start counter
 * @return DALI_Status DALI_OK if successful
 */
DALI_Status get_start_counter(uint8_t short_address, bit24_t *start_counter)
{
    uint8_t start_counter_MSB;
    uint8_t start_counter_2;
    uint8_t start_counter_LSB;
    DALI_Status dali_status;

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, START_COUNTER_MSB, &start_counter_MSB);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, START_COUNTER_2, &start_counter_2);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, START_COUNTER_LSB, &start_counter_LSB);
    if (dali_status != DALI_OK)
        return dali_status;

    *start_counter |= ((start_counter_MSB << 0x10U) | (start_counter_2 << 0x8U) | start_counter_LSB);

    return dali_status;
}

/**
 * @brief Get external supply voltage from DALI device
 *
 * @param short_address DALI device short address
 * @param external_supply_voltage Pointer to 16-bit value to store voltage
 * @return DALI_Status DALI_OK if successful
 */
DALI_Status get_external_supply_voltage(uint8_t short_address, uint16_t *external_supply_voltage)
{
    uint8_t external_supply_voltage_MSB;
    uint8_t external_supply_voltage_LSB;
    DALI_Status dali_status;

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_VOLTAGE_MSB, &external_supply_voltage_MSB);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_VOLTAGE_LSB, &external_supply_voltage_LSB);
    if (dali_status != DALI_OK)
        return dali_status;

    *external_supply_voltage |= (external_supply_voltage_MSB << 0x08) | (external_supply_voltage_LSB);

    return dali_status;
}

uint64_t read_manufactor_id(uint8_t short_address)
{
    uint8_t MSB;
    uint8_t manufactor_id_2;
    uint8_t manufactor_id_3;
    uint8_t manufactor_id_4;
    uint8_t manufactor_id_5;
    uint8_t manufactor_id_6;
    uint8_t manufactor_id_7;
    uint8_t LSB;

    read_memory_location(short_address, 0x00, 0x0E, &MSB);
    read_memory_location(short_address, 0x00, 0x0B, &manufactor_id_2);
    read_memory_location(short_address, 0x00, 0x0C, &manufactor_id_3);
    read_memory_location(short_address, 0x00, 0x0D, &manufactor_id_4);
    read_memory_location(short_address, 0x00, 0x0F, &manufactor_id_5);
    read_memory_location(short_address, 0x00, 0x11, &manufactor_id_6);
    read_memory_location(short_address, 0x00, 0x12, &manufactor_id_7);
    read_memory_location(short_address, 0x00, 0x13, &LSB);

    return ((uint64_t)MSB << 0x38U) |
           ((uint64_t)manufactor_id_2 << 0x30U) |
           ((uint64_t)manufactor_id_3 << 0x28U) |
           ((uint64_t)manufactor_id_4 << 0x20U) |
           ((uint64_t)manufactor_id_5 << 0x18U) |
           ((uint64_t)manufactor_id_6 << 0x10U) |
           ((uint64_t)manufactor_id_7 << 0x8U) |
           (uint64_t)LSB;
}

/**
 * @brief Print diagnostics data struct to console
 *
 * @param gear Controle_gear struct to print
 */
void printObject(Controle_gear gear)
{
    printf("\n\n");
    printf("Controle_gear {\n");
    printf("  operating_time: %lu\n", gear.operating_time);
    printf("  start_counter: %lu\n", gear.start_counter);
    printf("  external_supply_voltage: %u\n", gear.external_supply_voltage);
    printf("  external_supply_voltage_frequency: %u\n", gear.external_supply_voltage_frequency);
    printf("  power_factor: %u\n", gear.power_factor);
    printf("  overall_faliure_condition: %u\n", gear.overall_faliure_condition);
    printf("  external_supply_undervoltage: %u\n", gear.external_supply_undervoltage);
    printf("  external_supply_undervoltage_counter: %u\n", gear.external_supply_undervoltage_counter);
    printf("  external_supply_overvoltage: %u\n", gear.external_supply_overvoltage);
    printf("  external_supply_overvoltage_counter: %u\n", gear.external_supply_overvoltage_counter);
    printf("  output_power_limitation: %u\n", gear.output_power_limitation);
    printf("  output_power_limitation_counter: %u\n", gear.output_power_limitation_counter);
    printf("  thermal_derating: %u\n", gear.thermal_derating);
    printf("  thermal_derating_counter: %u\n", gear.thermal_derating_counter);
    printf("  thermal_shutdown: %u\n", gear.thermal_shutdown);
    printf("  thermal_shutdown_counter: %u\n", gear.thermal_shutdown_counter);
    printf("  temperature: %u\n", gear.temperature);
    printf("  output_current_percent: %u\n", gear.output_current_percent);
    printf("}\n");
}
