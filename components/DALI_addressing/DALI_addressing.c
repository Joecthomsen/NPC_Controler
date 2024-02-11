#include "DALI_addressing.h"
#include "DALI_commands.h"
#include <math.h>
#include "DALI_transmit.h"

// Prototypes
address24_t find_lowest_device_address(address24_t start, address24_t end);
bool compare_device_address(address24_t address);
void set_search_address(address24_t address);
void set_all_devices_in_initialize_state();
void generate_random_device_addresses();
void program_short_address(address24_t longAddress, uint8_t shortAddress);
bool verify_short_address(uint8_t shortAddress);

/**
 * @brief Commission all DALI drivers on the bus by assigning short addresses
 *
 * @return Number of drivers commissioned
 *
 * @warning This function should be called only if the system is not already
 * commissioned, as it will reset the short addresses of all drivers, potentially
 * causing issues if the driver is already commissioned.
 *
 * This function performs commissioning of all DALI drivers on the bus.
 * It assigns short addresses starting from 0 up to 63 sequentially.
 * The number of drivers commissioned is returned, so if 5 is returned,
 * addresses 0 to 4 have been assigned.
 */
uint8_t commission_bus()
{
    uint8_t counter = 0;
    address24_t address = 0;
    set_all_devices_in_initialize_state();
    generate_random_device_addresses();

    while (true)
    {
        address = find_lowest_device_address(address, 0xFFFFFF);
        if (address == 0xFFFFFF)
        {
            break;
        }
        vTaskDelay(DELAY_BETWEEN_COMMANDS);
        printf("Address found: %lx\n", address);
        program_short_address(address, counter);
        if (verify_short_address(counter))
        {
            printf("Short address verified\n");
        }
        else
        {
            printf("Short address not verified\n");
        }
        vTaskDelay(DELAY_BETWEEN_COMMANDS);
        sendDALI_TX(WITHDRAW); // Ensure that the search address is set on the driver, otherwise this will fail. In this case, the search address is set in the function set_search_address.
        vTaskDelay(DELAY_BETWEEN_COMMANDS);
        counter++;
    }
    sendDALI_TX(TERMINATE);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    return counter;
}

/**
 * @brief Check if any of the drivers on the bus are commisioned
 * @details This function checks if any of the drivers on the bus are commisioned, by iterating all the shord addresses and return true if any response.
 * @return true if there is any drivers on the bus commisioned, false otherwise
 */
DALI_Status check_drivers_commissioned()
{
    uint8_t totalDriversOnBus = 0;
    uint8_t driversWithShortAddressOnBus = 0;
    set_all_devices_in_initialize_state();
    generate_random_device_addresses();
    while (true)
    {
        address24_t address = find_lowest_device_address(0, 0xFFFFFF);
        if (address == 0xFFFFFF)
        {
            break;
        }
        vTaskDelay(DELAY_BETWEEN_COMMANDS);
        sendDALI_TX(WITHDRAW); // Ensure that the search address is set on the driver, otherwise this will fail. In this case, the search address is set in the function set_search_address.
        vTaskDelay(DELAY_BETWEEN_COMMANDS);
        totalDriversOnBus++;
    }
    sendDALI_TX(TERMINATE);

    for (int i = 0; i < 64; i++)
    {
        if (verify_short_address(i))
        {
            driversWithShortAddressOnBus++;
        }
    }
    if (totalDriversOnBus != driversWithShortAddressOnBus)
    {
        return DALI_ERR_UNCOMMISSIONED_DRIVER;
    }
    else if (totalDriversOnBus == 0)
    {
        return DALI_ERR_NO_DRIVERS;
    }
    else
    {
        return DALI_OK;
    }
}

/**
 * @brief Function to perform binary search for the lowest key
 * @param start The start of the search range, most likely 0, but can be any value
 * @param end The end of the search range, most likely the maximum key value for a uint24 (0xFFFFFF), but can be any value
 * @return The lowest key found in the search range
 */
address24_t find_lowest_device_address(address24_t start, address24_t end)
{
    printf("Searching for lowest address...\n");
    while (start < end)
    {
        address24_t mid = floor(start + (end - start) / 2);
        if (compare_device_address(mid))
        {
            end = mid; // Continue searching in the lower half
        }
        else
        {
            start = mid + 1; // Search in the upper half
        }
    }
    return start;
}

/**
 * @brief This function will set all the drivers on the bus to the initialization state
 */
void set_all_devices_in_initialize_state()
{
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    sendDALI_TX(INITIALIZE_ALL_DEVICE);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    sendDALI_TX(INITIALIZE_ALL_DEVICE);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
}

/**
 * @brief Generate random DALI addresses for all devices on bus
 *
 * This function causes all DALI devices on the bus to generate
 * random 24-bit addresses. It does this by sending two
 * GENERATE_RANDOM_ADDRESS commands with a delay between them.
 * The random addressing allows the bus to be commissioned by finding
 * the device with lowest address first.
 */
void generate_random_device_addresses()
{
    sendDALI_TX(GENERATE_RANDOM_ADDRESS);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    sendDALI_TX(GENERATE_RANDOM_ADDRESS);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
}

/**
 * @brief Compare a DALI address
 *
 * @param address The 24-bit address to compare against devices on bus
 * @return true if address is higher than the address of any device on the bus, false otherwise
 *
 * This function compares a given 24-bit address against all devices
 * on the DALI bus to check if it is higher than the address of any
 * device on the bus. It does this by setting the search address using
 * the given address, sending a COMPARE command, and checking if a
 * response is received. A false response indicates the address is
 * higher than any device address on the bus.
 */
bool compare_device_address(address24_t address)
{
    set_search_address(address);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    sendDALI_TX(COMPARE);
    vTaskDelay(DELAY_AWAIT_RESPONSE);

    return newDataAvailable() ? (clearNewDataFlag(), true) : false;
}

/**
 * @brief Program short address of a device given its long address
 *
 * @param longAddress The 24-bit long address of the device
 * @param shortAddress The 8-bit short address to program
 *
 * This function programs the short address of a DALI device given its
 * 24-bit long address. It does this by setting the search address to
 * the provided long address, then sending the PROGRAM_SHORT_ADDRESS
 * command with the short address to program. This assigns the short
 * address to the device with the matching long address.
 */
void program_short_address(address24_t longAddress, uint8_t shortAddress)
{
    set_search_address(longAddress);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    sendDALI_TX(PROGRAM_SHORT_ADDRESS | (shortAddress << 1));
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
}

bool verify_short_address(uint8_t shortAddress)
{
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    sendDALI_TX(VERIFY_SHORT_ADDRESS | (shortAddress << 1));
    vTaskDelay(DELAY_AWAIT_RESPONSE);
    return newDataAvailable() ? (clearNewDataFlag(), true) : false;
}

/**
 * @brief Set the DALI search address
 *
 * @param address The 24-bit address to set as the search address
 *
 * This function sets the 24-bit DALI search address that is used for
 * addressing and comparing devices on the bus. It splits the address
 * into high, middle and low bytes and sends the SEARCH_ADDRESS_H,
 * SEARCH_ADDRESS_M, and SEARCH_ADDRESS_L commands with each byte.
 */
void set_search_address(address24_t address)
{
    uint8_t hByte = (address >> 16) & 0xFF; // Get the high bytes
    uint8_t mByte = (address >> 8) & 0xFF;  // Get the middle bytes
    uint8_t lByte = (address >> 0) & 0xFF;  // Get the low bytes

    sendDALI_TX(SEARCH_ADDRESS_H + hByte);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    sendDALI_TX(SEARCH_ADDRESS_M + mByte);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
    sendDALI_TX(SEARCH_ADDRESS_L + lByte);
    vTaskDelay(DELAY_BETWEEN_COMMANDS);
}
