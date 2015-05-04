#include <pebble.h>
#include "timeSieve.h"
  
static Layer* s_timeSieveLayer;

static uint8_t s_sieveTickCount;

void sieveAnimReset() {
  s_sieveTickCount = 0;
}

bool sieveAnimCallback() {
  // TODO
  return false;
}

static void timeSieve_update_proc(Layer *this_layer, GContext *ctx) {
  GRect tank_bounds = layer_get_bounds(this_layer);
  
  // Fill back
  //graphics_context_set_fill_color(ctx, GColorLightGray);
  //graphics_fill_rect(ctx, tank_bounds, 2, GCornersAll);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(tank_bounds.origin.x + 20, tank_bounds.origin.y), GPoint(tank_bounds.size.w - 20, tank_bounds.origin.y) );
}

void create_timeSieve_layer(Window* parentWindow) {
  // Create the clock layer in the top 1/3 of the screen
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_timeSieveLayer = layer_create( GRect(0, window_bounds.size.h/3, window_bounds.size.w, window_bounds.size.h/3) );
  // Add as child of the main window layer and set callback
  layer_add_child(window_layer, s_timeSieveLayer);
  layer_set_update_proc(s_timeSieveLayer, timeSieve_update_proc); 
}

void destroy_timeSieve_layer() {
  layer_destroy(s_timeSieveLayer);
}