#include "DALI_addressing.h"
#include "DALI_commands.h"
#include <math.h>
#include "DALI_transmit.h"
#include "freertos/FreeRTOS.h"

// Prototypes
bool compareDALIAddress(address24_t address);

/**
 * @brief Check if any of the drivers on the bus are commisioned
 * @details This function checks if any of the drivers on the bus are commisioned, by iterating all the shord addresses and return true if any response.
 * @return true if there is any drivers on the bus commisioned, false otherwise
 */
bool areDriversOnBusCommisioned()
{
    // uint8_t i;
    //  for (i = 0; i < 256; i++){
    //      if (verifyShortAddress(i)){
    //          return true;
    //      }
    //  }
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
    while (start < end)
    {
        address24_t mid = floor(start + (end - start) / 2);
        printf("Mid: %ld\n", mid);
        if (compareDALIAddress(mid))
        {
            end = mid; // Continue searching in the lower half
            printf("True\n");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else
        {
            start = mid + 1; // Search in the upper half
            printf("False\n");
            // printf("Start: %lld\n", start);
        }
    }
    // counter = 0;
    sendDALI_TX(TERMINATE);
    return start;
}

bool programShortAddress(uint64_t longAddress, uint8_t shortAddress)
{
    return false;
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
    // printf("Before mask: 0x%lx\n", address);
    //  address24_t mask = 0xFFFFFF00; // 32 bit mask
    //  address &= mask;               // clear lower 8 bits

    uint8_t hByte = (address >> 16) & 0xFF; // Get the high bytes
    uint8_t mByte = (address >> 8) & 0xFF;  // Get the middle bytes
    uint8_t lByte = (address >> 0) & 0xFF;  // Get the low bytes

    printf("AddressSearch: 0x%lx\n", address);
    // printf("hByte: 0x%x\n", hByte);
    // printf("mByte: 0x%x\n", mByte);
    // printf("lByte: 0x%x\n", lByte);

    sendDALI_TX(SEARCH_ADDRESS_H + hByte);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
    sendDALI_TX(SEARCH_ADDRESS_M + mByte);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
    sendDALI_TX(SEARCH_ADDRESS_L + lByte);
    vTaskDelay(DELAY_BETWEEN_COMMANDS / portTICK_PERIOD_MS);
    sendDALI_TX(COMPARE);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    if (newDataAvailable())
    {
        printf("NEW DATA Hex value: %x\n", getNewData());
    }

    return newDataAvailable() ? (clearNewDataFlag(), true) : false;
}
