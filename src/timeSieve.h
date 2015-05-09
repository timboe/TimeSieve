#pragma once
#include <pebble.h>

  
void create_timeSieve_layer(Window* parentWindow);
void destroy_timeSieve_layer();

void sieveAnimReset(TimeUnits units_changed);
bool sieveAnimCallback(TimeUnits units_changed);

void displyItem(uint8_t treasureID, uint8_t itemID);
bool collectItem();
void stopDisplayItem(void* data);
void itemCanBeCollected();
