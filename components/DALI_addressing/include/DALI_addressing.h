#pragma once

#include <inttypes.h>
#include <stdbool.h>

int compareWithDriver(uint64_t key);
bool areDriversOnBusCommisioned();
uint64_t findLowestKey(uint64_t start, uint64_t end);
bool initialize(); 
bool randomize();
bool compare(uint64_t address);
bool programShortAddress(uint64_t longAddress, uint8_t shortAddress);
bool verifyShortAddress(uint8_t shortAddress);
bool withdraw(uint64_t address);