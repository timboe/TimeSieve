#pragma once
#include <pebble.h>
  

  
// Frame to hold tank gfx 
static Layer* s_tankLayer;

// // Buffers to hold displayed text
// static char* s_tankFullPercetText[16];
// static char* s_tankContentText[32];

// Layer render callback function prototype
static void timeTank_update_proc(Layer *this_layer, GContext *ctx);

void update_timeTank_layer();
void create_timeTank_layer(Window* parentWindow);
void destroy_timeTank_layer();