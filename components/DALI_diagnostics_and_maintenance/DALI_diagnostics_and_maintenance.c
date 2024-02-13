#include <stdio.h>
#include "DALI_diagnostics_and_maintenance.h"
#include "DALI_transmit.h"
#include "freertos/FreeRTOS.h"
#include "constants.h"

// Prototypes
DALI_Status read_memory_location(uint8_t short_address, uint8_t memory_bank, uint8_t location, uint8_t *data);
void select_memory_bank_location(uint8_t memory_bank, uint8_t location);
DALI_Status get_operating_time(uint8_t short_address, uint32_t *operating_time);
uint8_t calculate_short_address_standard_cmd(uint8_t short_address);

// Functions
Controle_gear fetch_controle_gear_data(uint8_t short_address)
{
    Controle_gear controle_gear;
    DALI_Status dali_status;

    uint32_t new_operating_time = 0;
    dali_status = get_operating_time(short_address, &new_operating_time);
    if (dali_status != DALI_OK)
        printf("Error: %d\n", dali_status);
    else
        printf("Operating time: %lu\n", new_operating_time);

    controle_gear.operating_time = new_operating_time;
    controle_gear.start_counter = 0;
    controle_gear.external_supply_voltage = 0;
    controle_gear.external_supply_voltage_frequency = 0;
    controle_gear.power_factor = 0;
    controle_gear.overall_faliure_condition = 0;
    controle_gear.external_supply_undervoltage = 0;
    controle_gear.external_supply_undervoltage_counter = 0;
    controle_gear.external_supply_overvoltage = 0;
    controle_gear.external_supply_overvoltage_counter = 0;
    controle_gear.output_power_limitation = 0;
    controle_gear.output_power_limitation_counter = 0;
    controle_gear.thermal_derating = 0;
    controle_gear.thermal_derating_counter = 0;
    controle_gear.thermal_shutdown = 0;
    controle_gear.thermal_shutdown_counter = 0;
    controle_gear.temperature = 0;
    controle_gear.output_current_percent = 0;

    return controle_gear;
}

DALI_Status read_memory_location(uint8_t short_address, uint8_t memory_bank, uint8_t location, uint8_t *data)
{
    select_memory_bank_location(memory_bank, location);
    sendDALI_TX(READ_MEMORY_LOCATION | (calculate_short_address_standard_cmd(short_address) << 8));
    vTaskDelay(DELAY_AWAIT_RESPONSE);
    if (newDataAvailable())
    {
        *data = getNewData();
        printf("DATA fetched: %x\n", *data);
        clearNewDataFlag();
    }
    else
    {
        return DALI_ERR_NO_RESPONSE;
        printf("No data available FROM OPERATING TIME\n");
    }
    return DALI_OK;
}

void select_memory_bank_location(uint8_t memory_bank, uint8_t location)
{
    sendDALI_TX(DTR0 + location);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    sendDALI_TX(DTR1 + memory_bank);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
}

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

    *operating_time |= ((operating_time_MSB << 0x18U) | (operating_time_2 << 0x10U) | (operating_time_3 << 0x8U) | operating_time_LSB); // The leftshifters are in HEX

    return dali_status;
}

// TODO maybe move this to DALI transmit?
uint8_t calculate_short_address_standard_cmd(uint8_t short_address)
{
    uint8_t result = 0;
    result = (short_address << 1) + 1;
    return result;
}
