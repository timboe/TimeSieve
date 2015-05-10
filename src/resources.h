#pragma once
#include <pebble.h>



GFont* getDOSFont();
GFont* getClockFont();
void init_resources();
void destroy_resources();

GBitmap* getItemImage(uint8_t treasureID, uint8_t itemID);
