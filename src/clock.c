#include <pebble.h>
#include <clock.h>
  
static Layer* s_clockLayer;

static void clock_update_proc(Layer *this_layer, GContext *ctx) {
  GRect tank_bounds = layer_get_bounds(this_layer);
  
  // Fill back
  graphics_context_set_fill_color(ctx, GColorBlue);
  graphics_fill_rect(ctx, tank_bounds, 2, GCornersAll);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_rect(ctx, tank_bounds);
}

void create_clock_layer(Window* parentWindow) {
  // Create the clock layer in the top 1/3 of the screen
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_clockLayer = layer_create( GRect(0, 0, window_bounds.size.w, window_bounds.size.h/3) );
  // Add as child of the main window layer and set callback
  layer_add_child(window_layer, s_clockLayer);
  layer_set_update_proc(s_clockLayer, clock_update_proc); 
}

void destroy_clock_layer() {
  layer_destroy(s_clockLayer);
}