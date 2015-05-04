#pragma once
#include <pebble.h>
  
void update_timeTank_layer();
void create_timeTank_layer(Window* parentWindow);
void destroy_timeTank_layer();

void tankAnimReset();
bool tankAnimCallback();