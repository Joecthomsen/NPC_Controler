#include <stdio.h>
#include "DALI_memory_bank_handler.h"
#include "DALI_communication.h"
#include "esp_log.h"

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
        clear_new_data_flag();
    }
    else
    {
        return DALI_ERR_NO_RESPONSE;
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
        clear_new_data_flag();
    }
    else
    {
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

/**
 * @brief Read the manufacturer ID.
 *
 * This function reads the manufacturer ID from the specified memory locations associated
 * with the given short address. It reads multiple bytes from memory locations and combines
 * them to form a 64-bit manufacturer ID.
 *
 * @param short_address The short address to read the manufacturer ID from.
 * @return The 64-bit manufacturer ID.
 */
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