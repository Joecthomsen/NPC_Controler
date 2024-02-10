#pragma once

#include <inttypes.h>
#include <stdbool.h>

bool areDriversOnBusCommisioned();
uint64_t findLowestAddress(uint64_t start, uint64_t end);

void initDALIAddressing();
void generateRandomDALIAddress();

bool programShortAddress(uint64_t longAddress, uint8_t shortAddress);
bool verifyShortAddress(uint8_t shortAddress);
bool withdraw(uint64_t address);