#pragma once
#include <pebble.h>
  
void update_timeTank_layer();
void create_timeTank_layer(Window* parentWindow);
void destroy_timeTank_layer();

void tankAnimReset(TimeUnits units_changed);
bool tankAnimCallback(TimeUnits units_changed);
