#include "DALI_addressing.h"
#include "DALI_commands.h"
#include <math.h>
#include "DALI_transmit.h"
#include "freertos/FreeRTOS.h"

// Prototypes
bool compareDALIAddress(address24_t address);
void setSearchAddress(address24_t address);

void commissionDALIBus(uint8_t *addressArray)
{

    uint8_t test[64];
    uint8_t counter = 0;
    address24_t address = 0;
    initDALIAddressing();
    generateRandomDALIAddress();

    while (true)
    {
        address = findLowestAddress(address, 0xFFFFFF);
        if (address == 0xFFFFFF)
        {
            printf("Drivers found: %d\n", counter);
            break;
        }
        setSearchAddress(address);
        printf("Address found: %lx\n", address);
        vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
        sendDALI_TX(PROGRAM_SHORT_ADDRESS + counter);
        vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
        sendDALI_TX(WITHDRAW);
        vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
        addressArray[counter] = counter;
        counter++;
    }
    sendDALI_TX(TERMINATE);
}

/**
 * @brief Check if any of the drivers on the bus are commisioned
 * @details This function checks if any of the drivers on the bus are commisioned, by iterating all the shord addresses and return true if any response.
 * @return true if there is any drivers on the bus commisioned, false otherwise
 */
bool areDriversOnBusCommisioned()
{
    return false;
}

/**
 * @brief Function to perform binary search for the lowest key
 * @param start The start of the search range, most likely 0, but can be any value
 * @param end The end of the search range, most likely the maximum key value for a uint24 (0xFFFFFF), but can be any value
 * @return The lowest key found in the search range
 */
address24_t findLowestAddress(address24_t start, address24_t end)
{
    printf("Searching for lowest address...\n");
    while (start < end)
    {
        address24_t mid = floor(start + (end - start) / 2);
        // printf("Mid: %ld\n", mid);
        if (compareDALIAddress(mid))
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

void initDALIAddressing()
{
    sendDALI_TX(TERMINATE);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
    sendDALI_TX(INITIALIZE_ALL_DEVICE);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
    sendDALI_TX(INITIALIZE_ALL_DEVICE);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
}

void generateRandomDALIAddress()
{
    sendDALI_TX(GENERATE_RANDOM_ADDRESS);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
    sendDALI_TX(GENERATE_RANDOM_ADDRESS);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
}

bool compareDALIAddress(address24_t address)
{
    setSearchAddress(address);
    sendDALI_TX(COMPARE);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    return newDataAvailable() ? (clearNewDataFlag(), true) : false;
}

bool programShortAddress(uint64_t longAddress, uint8_t shortAddress)
{
    return false;
}

void setSearchAddress(address24_t address)
{
    uint8_t hByte = (address >> 16) & 0xFF; // Get the high bytes
    uint8_t mByte = (address >> 8) & 0xFF;  // Get the middle bytes
    uint8_t lByte = (address >> 0) & 0xFF;  // Get the low bytes

    sendDALI_TX(SEARCH_ADDRESS_H + hByte);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
    sendDALI_TX(SEARCH_ADDRESS_M + mByte);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
    sendDALI_TX(SEARCH_ADDRESS_L + lByte);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
}
