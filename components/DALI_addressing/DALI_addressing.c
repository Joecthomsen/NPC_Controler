#include "DALI_addressing.h"
#include "DALI_commands.h"
#include <math.h>
#include "DALI_transmit.h"
#include "freertos/FreeRTOS.h"

// Prototypes
bool compareDALIAddress(uint64_t address);

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
 * @param end The end of the search range, most likely the maximum key value for a uint64 (0xFFFFFFFFFFFFFFFF), but can be any value
 * @return The lowest key found in the search range
 */
uint64_t findLowestAddress(uint64_t start, uint64_t end)
{
    while (start < end)
    {
        uint64_t mid = floor(start + (end - start) / 2);
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

bool programShortAddress(uint64_t longAddress, uint8_t shortAddress)
{
    return false;
}

void initDALIAddressing()
{
    sendDALI_TX(INITIALIZE_ALL_DEVICE);
    vTaskDelay(20 / portTICK_PERIOD_MS);
    sendDALI_TX(INITIALIZE_ALL_DEVICE);
    vTaskDelay(20 / portTICK_PERIOD_MS);
}

bool compareDALIAddress(uint64_t address)
{
    return true;
}