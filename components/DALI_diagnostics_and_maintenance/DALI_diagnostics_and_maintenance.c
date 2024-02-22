#include <stdio.h>
#include "DALI_diagnostics_and_maintenance.h"
#include "DALI_communication.h"
#include "freertos/FreeRTOS.h"
#include "constants.h"
#include "DALI_memory_bank_handler.h"
#include "esp_log.h"

static const char *TAG = "DALI_diagnostics_and_maintenance";

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
Controle_gear_values_t fetch_controle_gear_data(uint8_t short_address)
{
    Controle_gear_values_t controle_gear;
    DALI_Status dali_status;

    send_DALI_Tx(0x0181);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    send_DALI_Tx(0x0181);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);

    write_memory_location(short_address, MEMORY_BANK_205, LOCK_BYTE, 0x55);

    controle_gear.manufacturer_id = read_manufactor_id(short_address); // TODO implement error handling/logging

    dali_status = get_operating_time(short_address, &controle_gear.operating_time);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting operating time: %d\n", dali_status);

    dali_status = get_start_counter(short_address, &controle_gear.start_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting start counter: %d\n", dali_status);

    dali_status = get_external_supply_voltage(short_address, &controle_gear.external_supply_voltage);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting external supply voltage: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_VOLTAGE_FREQUENZY, &controle_gear.external_supply_voltage_frequency);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting external supply voltage frequenzy: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, POWER_FACTOR, &controle_gear.power_factor);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting power factor: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OVERALL_FALIURE_CONDITION, &controle_gear.overall_faliure_condition);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting overall faliure condition: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_UNDERVOLTAGE, &controle_gear.external_supply_undervoltage);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting external supply undervoltage: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_UNDERVOLTAGE_COUNTER, &controle_gear.external_supply_undervoltage_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting external supply undervoltage counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_OVERVOLTAGE, &controle_gear.external_supply_overvoltage);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting external supply overvoltage: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, EXTERNAL_SUPPLY_OVERVOLTAGE_COUNTER, &controle_gear.external_supply_overvoltage_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting external supply overvoltage counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OUTPUT_POWER_LIMITATION, &controle_gear.output_power_limitation);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting output power limitation: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OUTPUT_POWER_LIMITATION_COUNTER, &controle_gear.output_power_limitation_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting output power limitation counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, THERMAL_DERATING, &controle_gear.thermal_derating);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting thermal derating: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, THERMAL_DERATING_COUNTER, &controle_gear.thermal_derating_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting thermal derating counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, THERMAL_SHUTDOWN, &controle_gear.thermal_shutdown);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting thermal shutdown: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, THERMAL_SHUTDOWN_COUNTER, &controle_gear.thermal_shutdown_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting thermal shutdown counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, TEMPERATURE, &controle_gear.temperature);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting temperature: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_205, OUTPUT_CURRENT_PERCENT, &controle_gear.output_current_percent);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting output current percent: %d\n", dali_status);

    uint8_t locableByte = 0;
    dali_status = read_memory_location(short_address, MEMORY_BANK_205, 0x02, &locableByte);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting locable byte: %d\n", dali_status);

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

/**
 * @brief Print diagnostics data struct to console
 *
 * @param gear Controle_gear_values_t struct to print
 */
void printObject(Controle_gear_values_t gear)
{
    printf("\n\n");
    printf("Controle_gear_values_t {\n");
    printf("  manufacturer_id: %llu\n", gear.manufacturer_id);
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
