#pragma once
#include <pebble.h>

void mainWindowLoad(Window *window);
void mainWindowUnload(Window *window);

void mainWindowClickConfigProvider(Window *window);
void tapHandle(AccelData *data, uint32_t num_samples);

void animBegin();

void update_tick_handler();

TimeUnits getLastTimeUnit();
