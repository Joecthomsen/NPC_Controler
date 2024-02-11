#pragma once

#include <inttypes.h>
#include <stdbool.h>

#define DELAY_BETWEEN_COMMANDS 25 // 15 milliseconds

typedef uint32_t address24_t; // 24 bit address

bool areDriversOnBusCommisioned();
address24_t findLowestAddress(address24_t start, address24_t end);
void commissionDALIBus(uint8_t *addressArray);

void initDALIAddressing();
void generateRandomDALIAddress();

bool programShortAddress(uint64_t longAddress, uint8_t shortAddress);
bool verifyShortAddress(uint8_t shortAddress);
bool withdraw(uint64_t address);