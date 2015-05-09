#include <pebble.h>
#include "timeSieve.h"
#include "constants.h"
#include "persistence.h"

static Layer* s_timeSieveLayer;
static uint8_t s_sieveTickCount;

static GBitmap *s_convTopBitmap;
static GBitmap *s_convBotBitmap;
static GBitmap* s_convCap;
static uint8_t s_convOffset = 0;

static GBitmap* s_gemCommon;
static GBitmap* s_gemMagic;
static GBitmap* s_gemRare;
static GBitmap* s_gemEpic;
static GBitmap* s_gemLegendary;

static BitmapLayer* s_treasureLayer;
static GRect s_treasureFrame;
static bool s_treasureOnShow;
static int8_t s_treasureID;
static int8_t s_itemID;


// static BitmapLayer *s_convTopLayer;

void sieveAnimReset(TimeUnits units_changed) {
  s_sieveTickCount = 0;
  s_treasureFrame.origin.x = 95;
  layer_set_frame( bitmap_layer_get_layer(s_treasureLayer), s_treasureFrame );
}

bool sieveAnimCallback(TimeUnits units_changed) {
  // No animation if we are not moving a gem into place
  if (s_treasureID == -1) return false;

  if (++s_convOffset == 8) s_convOffset = 0; // Degenerency
  --s_treasureFrame.origin.x;
  layer_set_frame( bitmap_layer_get_layer(s_treasureLayer), s_treasureFrame );

  if (++s_sieveTickCount == ANIM_FRAMES) {
    itemCanBeCollected();
    return false;
  } else {
    return true; // Request more frames
  }
}

static void timeSieve_update_proc(Layer *this_layer, GContext *ctx) {
  GRect tank_bounds = layer_get_bounds(this_layer);
  
  // Fill back

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(tank_bounds.origin.x + 20, tank_bounds.origin.y), GPoint(tank_bounds.size.w - 20, tank_bounds.origin.y) );
  
  if (s_treasureOnShow) {
    switch (s_treasureID) {
      case COMMON_ID: graphics_context_set_stroke_color(ctx, COLOUR_COMMON); break;
      case MAGIC_ID: graphics_context_set_stroke_color(ctx, COLOUR_MAGIC); break;
      case RARE_ID: graphics_context_set_stroke_color(ctx, COLOUR_RARE); break;
      case EPIC_ID: graphics_context_set_stroke_color(ctx, COLOUR_EPIC); break;
      case LEGENDARY_ID: graphics_context_set_stroke_color(ctx, COLOUR_LEGENDARY); break;
    }
    graphics_context_set_stroke_width(ctx, 3);
    graphics_draw_circle(ctx, GPoint(52, 30), 9); 
    //graphics_context_set_stroke_width(ctx, 5);
    graphics_draw_circle(ctx, GPoint(52, 30), 14); 
    graphics_draw_circle(ctx, GPoint(52, 30), 19); 
    graphics_draw_circle(ctx, GPoint(52, 30), 24); 
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(20, 30, 80, 40), 0, GCornersAll); // Masking box
  }
  
  GRect convBotBound = GRect(0 + s_convOffset, 42, 94, 6);
  graphics_draw_bitmap_in_rect(ctx, s_convBotBitmap, convBotBound);

  GRect convTopBound = GRect(8 - s_convOffset, 30, 94, 12);
  graphics_draw_bitmap_in_rect(ctx, s_convTopBitmap, convTopBound);

  GRect convCapBound = GRect(0, 30, 11, 18);
  graphics_draw_bitmap_in_rect(ctx, s_convCap, convCapBound);

  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, GRect(90, 20, 40, 30), 0, GCornersAll);

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

  s_convTopBitmap = gbitmap_create_with_resource(RESOURCE_ID_CONV_TOP);
  s_convBotBitmap = gbitmap_create_with_resource(RESOURCE_ID_CONV_BOT);
  s_convCap = gbitmap_create_with_resource(RESOURCE_ID_CONV_CAP);

  s_gemCommon = gbitmap_create_with_resource(RESOURCE_ID_GEM_COMMON);
  s_gemMagic = gbitmap_create_with_resource(RESOURCE_ID_GEM_MAGIC); 
  s_gemRare = gbitmap_create_with_resource(RESOURCE_ID_GEM_RARE); 
  s_gemEpic = gbitmap_create_with_resource(RESOURCE_ID_GEM_EPIC); 
  s_gemLegendary = gbitmap_create_with_resource(RESOURCE_ID_GEM_LEGENDARY); 

  s_treasureID = -1;
  s_treasureFrame = GRect(95, 23, 15, 15);
  s_treasureLayer = bitmap_layer_create(s_treasureFrame);
  bitmap_layer_set_compositing_mode(s_treasureLayer, GCompOpSet); // W transparencies
  layer_add_child(s_timeSieveLayer, bitmap_layer_get_layer(s_treasureLayer));
}

void stopDisplayItem(void* data) {
  bitmap_layer_set_bitmap(s_treasureLayer, NULL);
  s_treasureOnShow = false;
  s_treasureID = -1;
}

void itemCanBeCollected() {
  s_treasureOnShow = true;
  app_timer_register(TREASURE_DISPLAY_TIME, stopDisplayItem, NULL);   // Start timer to remove treasure
}

void displyItem(uint8_t treasureID, uint8_t itemID) {
  s_treasureID = treasureID;
  s_itemID = itemID;
  // If no animation then pop straight in
  switch (treasureID) {
    case COMMON_ID: bitmap_layer_set_bitmap(s_treasureLayer, s_gemCommon); break;
    case MAGIC_ID: bitmap_layer_set_bitmap(s_treasureLayer, s_gemMagic); break;
    case RARE_ID: bitmap_layer_set_bitmap(s_treasureLayer, s_gemRare); break;
    case EPIC_ID: bitmap_layer_set_bitmap(s_treasureLayer, s_gemEpic); break;
    case LEGENDARY_ID: bitmap_layer_set_bitmap(s_treasureLayer, s_gemLegendary); break;
  }
  
  // No animation? pop right in
  if ( getUserOpt(OPT_ANIMATE) == false ) {
    s_treasureFrame.origin.x = 45;
    layer_set_frame( bitmap_layer_get_layer(s_treasureLayer), s_treasureFrame );
    itemCanBeCollected();
  }
}

bool collectItem() {
  if (s_treasureOnShow == false) return false;
  addItem(s_treasureID, s_itemID, 1);
  stopDisplayItem(NULL);
  // TODO add collect window
  return true;
}

void destroy_timeSieve_layer() {
  layer_destroy(s_timeSieveLayer);
  bitmap_layer_destroy(s_treasureLayer);

  gbitmap_destroy(s_convTopBitmap);
  gbitmap_destroy(s_convBotBitmap);
  gbitmap_destroy(s_convCap);

  gbitmap_destroy(s_gemCommon);
  gbitmap_destroy(s_gemMagic);
  gbitmap_destroy(s_gemRare);
  gbitmap_destroy(s_gemEpic);
  gbitmap_destroy(s_gemLegendary);
}
