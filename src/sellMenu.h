#pragma once
#include <pebble.h>

void sell_window_load(Window* parentWindow);
void sell_window_unload();

void updateSellLayer();

int8_t getItemIDFromRow(const unsigned treasureID, const uint16_t row);
int8_t getAchievementIDFromRow(const uint16_t row);
