#pragma once
#include <pebble.h>


void draw3DText(GContext *ctx, GRect loc, GFont* f, char* buffer, uint8_t offset, bool BWMode, GColor BWFg, GColor BWBg);

void create_clock_layer(Window* parentWindow);
void destroy_clock_layer();
void updateClockLayer();

void clockAnimReset(TimeUnits units_changed);
bool clockAnimCallback(TimeUnits units_changed);

void setClockPixelOffset(uint8_t offset);
void updateTimeBuffer();
void updateDateBuffer();
void updateBattery(BatteryChargeState charge);
