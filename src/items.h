#pragma once
#include <pebble.h>

uint8_t getItemRarity(TimeUnits units_changed);

int8_t checkForItem(TimeUnits units_changed);

bool getItemAutoCollect();

uint16_t sellItem(const unsigned treasureID, const unsigned itemID, bool sellAll);

const char* getItemName(uint8_t treasureID, uint8_t itemID);
