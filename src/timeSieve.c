#include <pebble.h>
#include "timeSieve.h"
#include "constants.h"

static Layer* s_timeSieveLayer;

static uint8_t s_sieveTickCount;

static GBitmap *s_convTopBitmap;
static GBitmap *s_convBotBitmap;

static uint8_t s_convOffset = 0;

// static BitmapLayer *s_convTopLayer;

void sieveAnimReset() {
  s_sieveTickCount = 0;
}

bool sieveAnimCallback() {

  if (++s_convOffset == 9) s_convOffset = 0; // Degenerency

  if (++s_sieveTickCount == ANIM_FRAMES) {
    return false;
  } else {
    return true; // Request more frames
  }
}

#define N_CONV 13

static void timeSieve_update_proc(Layer *this_layer, GContext *ctx) {
  GRect tank_bounds = layer_get_bounds(this_layer);
  
  // Fill back
  //graphics_context_set_fill_color(ctx, GColorLightGray);
  //graphics_fill_rect(ctx, tank_bounds, 2, GCornersAll);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(tank_bounds.origin.x + 20, tank_bounds.origin.y), GPoint(tank_bounds.size.w - 20, tank_bounds.origin.y) );
  
  GRect convBotBound = GRect(10 - 8 + s_convOffset, 25, 10, 12);
  for (uint8_t i = 0; i < N_CONV; ++i) {
    graphics_draw_bitmap_in_rect(ctx, s_convBotBitmap, convBotBound);
    convBotBound.origin.x += 8;
  }

  GRect convTopBound = GRect(10 - s_convOffset, 20, 100, 12);
  for (uint8_t i = 0; i < N_CONV; ++i) {
    graphics_draw_bitmap_in_rect(ctx, s_convTopBitmap, convTopBound);
    convBotBound.origin.x += 8;
  }
}

void create_timeSieve_layer(Window* parentWindow) {
  // Create the clock layer in the top 1/3 of the screen
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect window_bounds = layer_get_bounds(window_layer);
  GRect layerBounds = GRect(0, window_bounds.size.h/3, window_bounds.size.w, window_bounds.size.h/3);
  s_timeSieveLayer = layer_create( layerBounds );
  // Add as child of the main window layer and set callback
  layer_add_child(window_layer, s_timeSieveLayer);
  layer_set_update_proc(s_timeSieveLayer, timeSieve_update_proc); 

  //GRect convTopBound = GRect(10, 20, 100, 12);
  s_convTopBitmap = gbitmap_create_with_resource(RESOURCE_ID_CONV_TOP);
  s_convBotBitmap = gbitmap_create_with_resource(RESOURCE_ID_CONV_BOT);

  //s_convTopLayer = bitmap_layer_create(convTopBound);
  //bitmap_layer_set_bitmap(s_convTopLayer, s_convTopBitmap);
  //bitmap_layer_set_alignment(s_convTopLayer, GAlignLeft);
  //bitmap_layer_set_compositing_mode(s_convTopLayer, GCompOpSet);
  //layer_add_child(s_timeSieveLayer, bitmap_layer_get_layer(s_convTopLayer));
}

void destroy_timeSieve_layer() {
  layer_destroy(s_timeSieveLayer);
  ///bitmap_layer_destroy(s_convTopLayer);
  gbitmap_destroy(s_convTopBitmap);
  gbitmap_destroy(s_convBotBitmap);

}