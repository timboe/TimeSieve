#pragma once
#include <pebble.h>

void sellWindowLoad(Window* parentWindow);
void sellWindowUnload();

void updateSellLayer();

int8_t getItemIDFromRow(const unsigned treasureID, const uint16_t row);
int8_t getAchievementIDFromRow(const uint16_t row);
