#pragma once
#include <pebble.h>
#include "constants.h"

void init_timeStore();
void destroy_timeStore();

uint64_t safeAdd(uint64_t a, uint64_t b);
uint64_t safeMultiply(uint64_t a, uint64_t b);

void timeToString(uint64_t time, char* buffer, size_t buffer_size, bool brief);
void percentageToString(uint64_t amount, uint64_t total, char* buffer, const size_t buffer_size, unsigned* value, bool extraDigits);

uint64_t getPriceOfUpgrade(const uint32_t typeID, const uint32_t resourceID);
uint64_t getPriceOfNext(uint64_t priceOfCurrent, uint32_t typeID);

uint64_t doPurchase(const uint32_t typeID, const uint32_t resourceID);
bool getIfAtMaxLevel(uint32_t typeID, uint32_t resourceID);

uint64_t getTimePerMin();
void updateTimePerMin();

uint64_t getTankCapacity();
void updateTankCapacity();

void modulateSellPrices();
uint64_t getCurrentSellPrice(const uint32_t treasureID, const uint32_t itemID);
void currentSellPricePercentage(char* buffer, const size_t buffer_size, unsigned* value, const uint32_t treasureID, const uint32_t itemID);
uint64_t currentCategorySellPrice(const uint32_t treasureID);
uint64_t currentTotalSellPrice();

void addTime(uint64_t toAdd);
void removeTime(uint64_t toSubtract);

uint64_t getDisplayTime();
void updateDisplayTime(uint64_t t);
