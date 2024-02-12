#include <stdio.h>
#include "DALI_diagnostics_and_maintenance.h"
#include "DALI_transmit.h"
#include "freertos/FreeRTOS.h"

// Prototypes
void select_memory_bank_location(uint8_t memory_bank, uint8_t location);
uint32_t get_operating_time(uint8_t short_address);
uint8_t calculate_short_address_standard_cmd(uint8_t short_address);

// Functions
Controle_gear fetch_controle_gear_data(uint8_t short_address)
{
    Controle_gear controle_gear;

    controle_gear.operating_time = get_operating_time(short_address);
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

void select_memory_bank_location(uint8_t memory_bank, uint8_t location)
{
    sendDALI_TX(DTR0 + location);
    vTaskDelay(25 / portTICK_PERIOD_MS);
    sendDALI_TX(DTR1 + memory_bank);
    vTaskDelay(25 / portTICK_PERIOD_MS);
}

uint32_t get_operating_time(uint8_t short_address)
{
    uint8_t operating_time_MSB;
    uint8_t operating_time_2;
    uint8_t operating_time_3;
    uint8_t operating_time_LSB;

    select_memory_bank_location(MEMORY_BANK_205, OPERATING_TIME_MSB);
    vTaskDelay(25 / portTICK_PERIOD_MS);
    sendDALI_TX(READ_MEMORY_LOCATION | (calculate_short_address_standard_cmd(short_address) << 8));
    vTaskDelay(50 / portTICK_PERIOD_MS);
    if (newDataAvailable())
    {
        operating_time_MSB = getNewData();
        clearNewDataFlag();
    }
    else
    {
        printf("No data available FROM OPERATING TIME\n");
    }
    return 0;
}

uint8_t calculate_short_address_standard_cmd(uint8_t short_address)
{
    uint8_t result = 0;
    result = (short_address << 1) + 1;
    return result;
}
