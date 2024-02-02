#include "DALI_addressing.h"


/**
 * @brief Mock function for comparison with your ESP32 response
 *
 * @param addressSearch The address to search for
 * @return 1 if the address is found, 0 otherwise
 * @attention This function should be replaced with the actual implementation of the ESP32 and DALI driver
 */
int compareWithESP32(uint64_t addressSearch) {
    // Mock implementation: Return 1 for keys greater than or equal to a threshold
    return addressSearch >= 987654 ? 1 : 0;
}

/**
 * @brief Function to perform binary search for the lowest key
 * @param start The start of the search range, most likely 0, but can be any value
 * @param end The end of the search range, most likely the maximum key value for a uint64 (0xFFFFFFFFFFFFFFFF), but can be any value
 * @return The lowest key found in the search range
 */
uint64_t findLowestKey(uint64_t start, uint64_t end) {
    while (start < end) {
        uint64_t mid = start + (end - start) / 2;

        if (compareWithESP32(mid)) {
            end = mid;  // Continue searching in the lower half
        } else {
            start = mid + 1;  // Search in the upper half
        }
    }

    return start;
}