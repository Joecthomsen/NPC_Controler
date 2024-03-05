#include <stdio.h>
#include "DALI_diagnostics_and_maintenance.h"
#include "DALI_communication.h"
#include "freertos/FreeRTOS.h"
#include "constants.h"
#include "DALI_memory_bank_handler.h"
#include "esp_log.h"

static const char *TAG = "DALI_diagnostics_and_maintenance";

DALI_Status read_2_bytes(uint8_t short_address, uint16_t *return_value, uint8_t memory_bank, uint8_t start_address);
DALI_Status read_3_bytes(uint8_t short_address, bit24_t *return_value, uint8_t memory_bank, uint8_t start_address);
DALI_Status read_4_bytes(uint8_t short_address, uint32_t *return_value, uint8_t memory_bank, uint8_t start_address);
uint8_t calculate_short_address_standard_cmd(uint8_t short_address);

/**
 * @brief Fetch diagnostics data from a DALI control gear
 *
 * This function reads diagnostics data from the specified DALI control gear's
 * memory banks 205-207. It reads operating hours, start counters, voltages,
 * currents, temperatures, failure counters, and other metrics.
 *
 * @param short_address DALI control gear short address to read from
 * @return Controle_gear_values_t Struct containing the retrieved diagnostics data
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
    controle_gear.short_address = short_address;

    // Fetch memory bank 205

    dali_status = read_4_bytes(short_address, &controle_gear.operating_time, MEMORY_BANK_205, OPERATING_TIME_MSB); // get_operating_time(short_address, &controle_gear.operating_time);
    // dali_status = get_operating_time(short_address, &controle_gear.operating_time);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting operating time: %d\n", dali_status);

    dali_status = read_3_bytes(short_address, &controle_gear.start_counter, MEMORY_BANK_205, START_COUNTER_MSB);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting start counter: %d\n", dali_status);

    dali_status = read_2_bytes(short_address, &controle_gear.external_supply_voltage, MEMORY_BANK_205, EXTERNAL_SUPPLY_VOLTAGE_MSB);
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

    // Fetch memory bank 206

    dali_status = read_3_bytes(short_address, &controle_gear.light_source_start_counter_resettable, MEMORY_BANK_206, LIGHT_SOURCE_START_COUNTER_RESETTABLE_MSB);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source start counter: %d\n", dali_status);

    dali_status = read_3_bytes(short_address, &controle_gear.light_source_start_counter, MEMORY_BANK_206, LIGHT_SOURCE_START_COUNTER_MSB);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source start counter: %d\n", dali_status);

    dali_status = read_4_bytes(short_address, &controle_gear.light_source_on_time_resettable, MEMORY_BANK_206, LIGHT_SOURCE_ON_TIME_RESETTABLE_MSB);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source on time: %d\n", dali_status);

    dali_status = read_4_bytes(short_address, &controle_gear.light_source_on_time, MEMORY_BANK_206, LIGHT_SOURCE_ON_TIME_MSB);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source on time: %d\n", dali_status);

    dali_status = read_2_bytes(short_address, &controle_gear.light_source_voltage, MEMORY_BANK_206, LIGHT_SOURCE_VOLTAGE_MSB);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source voltage: %d\n", dali_status);

    dali_status = read_2_bytes(short_address, &controle_gear.light_source_current, MEMORY_BANK_206, LIGHT_SOURCE_CURRENT_MSB);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source current: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_OVERALL_FALIURE_CONDITION, &controle_gear.light_source_overall_faliure_condition);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source overall faliure condition: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_OVERALL_FALIURE_CONDITION_COUNTER, &controle_gear.light_source_overall_faliure_condition_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source overall faliure condition counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_SHORT_CIRCUIT, &controle_gear.light_source_short_circuit);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source short circuit: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_SHORT_CIRCUIT_COUNTER, &controle_gear.light_source_short_circuit_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source short circuit counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_OPEN_CIRCUIT, &controle_gear.light_source_open_circuit);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source open circuit: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_OPEN_CIRCUIT_COUNTER, &controle_gear.light_source_open_circuit_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source open circuit counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_THERMAL_DERATING, &controle_gear.light_source_thermal_derating);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source thermal derating: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_THERMAL_DERATING_COUNTER, &controle_gear.light_source_thermal_derating_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source thermal derating counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_THERMAL_SHUTDOWN, &controle_gear.light_source_thermal_shutdown);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source thermal shutdown: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_THERMAL_SHUTDOWN_COUNTER, &controle_gear.light_source_thermal_shutdown_counter);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source thermal shutdown counter: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_206, LIGHT_SOURCE_TEMPERATURE, &controle_gear.light_source_temperature);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting light source temperature: %d\n", dali_status);

    // Fetch memory bank 207
    dali_status = read_memory_location(short_address, MEMORY_BANK_207, RATED_MEDIAN_USEFULL_LIFE_OF_LUMINARE, &controle_gear.rated_median_usefull_life_of_luminare);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting rated median usefull life of luminares: %d\n", dali_status);

    dali_status = read_memory_location(short_address, MEMORY_BANK_207, INTERNAL_CONTROLE_GEAR_REFERENCE_TEMPERATURE, &controle_gear.internal_controle_gear_reference_temperature);
    if (dali_status != DALI_OK)
        ESP_LOGE(TAG, "Error getting internal controle gear reference temperature: %d\n", dali_status);

    dali_status = read_2_bytes(short_address, &controle_gear.rated_median_usefull_light_source_starts, MEMORY_BANK_207, RATED_MEDIAN_USEFULL_LIGHT_SOURCE_STARTS_MSB);

    return controle_gear;
}

/**
 * @brief Read a 2 byte value from a DALI control gear memory bank
 *
 * This function reads a 16-bit value from the specified memory bank and start address
 * of the given DALI control gear. It reads the MSB and LSB and combines them into
 * the 16-bit return value.
 *
 * @param short_address DALI control gear short address to read from
 * @param return_value Pointer to 16-bit value to store the read data
 * @param memory_bank Memory bank number to read from
 * @param start_address Start address in the memory bank to begin reading
 * @return DALI_Status DALI_OK if successful read
 */
DALI_Status read_2_bytes(uint8_t short_address, uint16_t *return_value, uint8_t memory_bank, uint8_t start_address)
{
    uint8_t MSB;
    uint8_t LSB;
    DALI_Status dali_status;

    dali_status = read_memory_location(short_address, memory_bank, start_address, &MSB);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, memory_bank, start_address + 1, &LSB);
    if (dali_status != DALI_OK)
        return dali_status;

    *return_value = (MSB << 0x08) | (LSB);

    return dali_status;
}

/**
 * @brief Read a 3 byte value from a DALI control gear memory bank
 *
 * This function reads a 24-bit value from the specified memory bank and start address
 * of the given DALI control gear. It reads the MSB, middle byte, and LSB and combines
 * them into the 32-bit return value (typedef'd as bit24_t).
 *
 * @param short_address DALI control gear short address to read from
 * @param return_value Pointer to 32-bit value (typedef'd as bit24_t) to store the read data
 * @param memory_bank Memory bank number to read from
 * @param start_address Start address in the memory bank to begin reading
 * @return DALI_Status DALI_OK if successful read
 */
DALI_Status read_3_bytes(uint8_t short_address, bit24_t *return_value, uint8_t memory_bank, uint8_t start_address)
{
    uint8_t MSB;
    uint8_t byte_2;
    uint8_t LSB;
    DALI_Status dali_status;

    dali_status = read_memory_location(short_address, memory_bank, start_address, &MSB);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, memory_bank, start_address + 1, &byte_2);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, memory_bank, start_address + 2, &LSB);
    if (dali_status != DALI_OK)
        return dali_status;

    *return_value = ((MSB << 0x10U) | (byte_2 << 0x8U) | LSB);

    return dali_status;
}

/**
 * @brief Read a 4 byte value from a DALI control gear memory bank
 *
 * This function reads a 32-bit value from the specified memory bank and start address
 * of the given DALI control gear. It reads the MSB through LSB and combines them
 * into the 32-bit return value.
 *
 * @param short_address DALI control gear short address to read from
 * @param return_value Pointer to 32-bit value to store the read data
 * @param memory_bank Memory bank number to read from
 * @param start_address Start address in the memory bank to begin reading
 * @return DALI_Status DALI_OK if successful read
 */
DALI_Status read_4_bytes(uint8_t short_address, uint32_t *return_value, uint8_t memory_bank, uint8_t start_address)
{
    uint8_t MSB = 0;
    uint8_t second = 0;
    uint8_t third = 0;
    uint8_t LSB = 0;

    DALI_Status dali_status;

    dali_status = read_memory_location(short_address, memory_bank, start_address, &MSB);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, memory_bank, start_address + 1, &second);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, memory_bank, start_address + 2, &third);
    if (dali_status != DALI_OK)
        return dali_status;

    dali_status = read_memory_location(short_address, memory_bank, start_address + 3, &LSB);
    if (dali_status != DALI_OK)
        return dali_status;

    *return_value = ((MSB << 0x18U) | (second << 0x10U) | (third << 0x8U) | LSB);

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
    printf("  Manufacturer ID: %llu\n", gear.manufacturer_id);
    printf("  Operating Time: %lu\n", gear.operating_time);
    printf("  Start Counter: %lu\n", gear.start_counter);
    printf("  External Supply Voltage: %u\n", gear.external_supply_voltage);
    printf("  External Supply Voltage Frequency: %u\n", gear.external_supply_voltage_frequency);
    printf("  Power Factor: %u\n", gear.power_factor);
    printf("  Overall Failure Condition: %u\n", gear.overall_faliure_condition);
    printf("  External Supply Undervoltage: %u\n", gear.external_supply_undervoltage);
    printf("  External Supply Undervoltage Counter: %u\n", gear.external_supply_undervoltage_counter);
    printf("  External Supply Overvoltage: %u\n", gear.external_supply_overvoltage);
    printf("  External Supply Overvoltage Counter: %u\n", gear.external_supply_overvoltage_counter);
    printf("  Output Power Limitation: %u\n", gear.output_power_limitation);
    printf("  Output Power Limitation Counter: %u\n", gear.output_power_limitation_counter);
    printf("  Thermal Derating: %u\n", gear.thermal_derating);
    printf("  Thermal Derating Counter: %u\n", gear.thermal_derating_counter);
    printf("  Thermal Shutdown: %u\n", gear.thermal_shutdown);
    printf("  Thermal Shutdown Counter: %u\n", gear.thermal_shutdown_counter);
    printf("  Temperature: %u\n", gear.temperature);
    printf("  Output Current Percent: %u\n", gear.output_current_percent);
    printf("  Light Source Start Counter Resettable: %lu\n", gear.light_source_start_counter_resettable);
    printf("  Light Source Start Counter: %lu\n", gear.light_source_start_counter);
    printf("  Light Source On Time Resettable: %lu\n", gear.light_source_on_time_resettable);
    printf("  Light Source On Time: %lu\n", gear.light_source_on_time);
    printf("  Light Source Voltage: %u\n", gear.light_source_voltage);
    printf("  Light Source Current: %u\n", gear.light_source_current);
    printf("  Light Source Overall Failure Condition: %u\n", gear.light_source_overall_faliure_condition);
    printf("  Light Source Overall Failure Condition Counter: %u\n", gear.light_source_overall_faliure_condition_counter);
    printf("  Light Source Short Circuit: %u\n", gear.light_source_short_circuit);
    printf("  Light Source Short Circuit Counter: %u\n", gear.light_source_short_circuit_counter);
    printf("  Light Source Open Circuit: %u\n", gear.light_source_open_circuit);
    printf("  Light Source Open Circuit Counter: %u\n", gear.light_source_open_circuit_counter);
    printf("  Light Source Thermal Derating: %u\n", gear.light_source_thermal_derating);
    printf("  Light Source Thermal Derating Counter: %u\n", gear.light_source_thermal_derating_counter);
    printf("  Light Source Thermal Shutdown: %u\n", gear.light_source_thermal_shutdown);
    printf("  Light Source Thermal Shutdown Counter: %u\n", gear.light_source_thermal_shutdown_counter);
    printf("  Light Source Temperature: %u\n", gear.light_source_temperature);
    printf("  Rated Median Useful Life of Luminaire: %u\n", gear.rated_median_usefull_life_of_luminare);
    printf("  Internal Controle Gear Reference Temperature: %u\n", gear.internal_controle_gear_reference_temperature);
    printf("  Rated Median Useful Light Source Starts: %u\n", gear.rated_median_usefull_light_source_starts);
    printf("}\n");
}
