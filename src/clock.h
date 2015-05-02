#pragma once
#include <pebble.h>



void create_clock_layer(Window* parentWindow);
void destroy_clock_layer();

void animCallback();

bool spoogeAnimation();