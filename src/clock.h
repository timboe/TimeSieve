#pragma once
#include <pebble.h>



void create_clock_layer(Window* parentWindow);
void destroy_clock_layer();
void updateClockLayer();

void clockAnimReset();
bool clockAnimCallback();


void setClockPixelOffset(uint8_t offset);