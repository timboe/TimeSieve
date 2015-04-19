#pragma once
#include <pebble.h>

static Layer* s_clockLayer;
  
  
static void clock_update_proc(Layer *this_layer, GContext *ctx);

void create_clock_layer(Window* parentWindow);
void destroy_clock_layer();