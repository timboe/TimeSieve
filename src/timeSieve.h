#pragma once
#include <pebble.h>

static Layer* s_timeSieveLayer;
  
static void timeSieve_update_proc(Layer *this_layer, GContext *ctx);

void create_timeSieve_layer(Window* parentWindow);
void destroy_timeSieve_layer();