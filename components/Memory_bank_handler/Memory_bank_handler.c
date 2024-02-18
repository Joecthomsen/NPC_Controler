#include <stdio.h>
#include "Memory_bank_handler.h"
#include "DALI_communication.h"

void select_memory_bank_location(uint8_t memory_bank, uint8_t location);
uint8_t calculate_short_address_standard_cmd(uint8_t short_address);

/**
 * @brief Read a memory location from a DALI device
 *
 * @param short_address DALI device short address
 * @param memory_bank Memory bank to read from
 * @param location Location within memory bank
 * @param data Pointer to store the read data
 * @return DALI_Status DALI_OK if successful
 */
DALI_Status read_memory_location(uint8_t short_address, uint8_t memory_bank, uint8_t location, uint8_t *data)
{
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    select_memory_bank_location(memory_bank, location);
    send_DALI_Tx(READ_MEMORY_LOCATION | (calculate_short_address_standard_cmd(short_address) << 8));
    vTaskDelay(DELAY_AWAIT_RESPONSE);
    if (new_data_available())
    {
        *data = get_new_data();
        printf("DATA fetched: %x\n", *data);
        clear_new_data_flag();
    }
    else
    {
        return DALI_ERR_NO_RESPONSE;
        printf("No data available FROM OPERATING TIME\n");
    }
    return DALI_OK;
}

/**
 * @brief Write data to a memory location on a DALI device
 *
 * @param short_address DALI device short address
 * @param memory_bank Memory bank to write to
 * @param location Location within memory bank
 * @param data Data byte to write
 * @return DALI_Status DALI_OK if successful
 */
DALI_Status write_memory_location(uint8_t short_address, uint8_t memory_bank, uint8_t location, uint8_t data)
{
    DALI_Status dali_status = DALI_OK;
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    select_memory_bank_location(memory_bank, location);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    send_DALI_Tx(WRITE_MEMORY_LOCATION + data);
    vTaskDelay(DELAY_AWAIT_RESPONSE);
    if (new_data_available())
    {
        printf("NEW DATA: %x\n", get_new_data());
        clear_new_data_flag();
    }
    else
    {
        printf("No data available FROM WRITE MEMORY\n");
        dali_status = DALI_ERR_NO_RESPONSE;
    }
    return dali_status;
}

/**
 * @brief Select a DALI device memory bank and location
 *
 * @param memory_bank Memory bank to select
 * @param location Location within memory bank to select
 */
void select_memory_bank_location(uint8_t memory_bank, uint8_t location)
{
    send_DALI_Tx(DTR0 + location);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    send_DALI_Tx(DTR1 + memory_bank);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
}

// TODO maybe move this to DALI transmit?
/**
 * @brief Calculate short address command for standard DALI commands
 *
 * @param short_address DALI device short address
 * @return uint8_t Short address command byte
 */
uint8_t calculate_short_address_standard_cmd(uint8_t short_address)
{
    uint8_t result = 0;
    result = (short_address << 1) + 1;
    return result;
}