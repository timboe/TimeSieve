#pragma once
#include <pebble.h>

  
void create_timeSieve_layer(Window* parentWindow);
void destroy_timeSieve_layer();

void sieveAnimReset();
bool sieveAnimCallback();