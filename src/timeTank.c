#include <pebble.h>
#include "timeTank.h"
#include "timeStore.h"
#include "constants.h"
#include "resources.h"
#include "ditheredRect.h"
#include "persistence.h"
  
// Frame to hold tank gfx 
static Layer* s_tankLayer;

void update_timeTank_layer() {
  layer_mark_dirty(s_tankLayer);
}  
  
/**
 * Draw the TimeTank which occupies the bottom 1/3 of the screen and holds how much liquid time the user has
 * along with how full the tank is
 */
static void timeTank_update_proc(Layer *this_layer, GContext *ctx) {
  GRect tank_bounds = layer_get_bounds(this_layer);
  
  static char s_tankFullPercetText[TEXT_BUFFER_SIZE];
  static char s_tankContentText[TEXT_BUFFER_SIZE];
  
  unsigned _percentage;
  percentage_to_string(getUserTime(), getTankCapacity(), s_tankFullPercetText, &_percentage);
  time_to_string(getUserTime(), s_tankContentText, TEXT_BUFFER_SIZE, true);
  
  // Fill back
  graphics_context_set_fill_color(ctx, GColorLightGray);
  //graphics_fill_rect(ctx, tank_bounds, 10, GCornersAll);
  draw_gradient_rect(ctx, tank_bounds,GColorDarkGray, GColorLightGray, TOP_TO_BOTTOM);

  // Fill liquid height
  int liquid_height = (tank_bounds.size.h * _percentage) / 100;
  GRect liquid_rect = GRect(0, tank_bounds.origin.y + tank_bounds.size.h - liquid_height, tank_bounds.size.w, liquid_height);
  graphics_context_set_fill_color(ctx, GColorBlueMoon);
  graphics_fill_rect(ctx, liquid_rect, 0, GCornersAll);
  
  // Fill bits in the liquid
  static GPoint liquid_bits_bgn[N_LIQUID_BITS];
  static GPoint liquid_bits_end[N_LIQUID_BITS];
  static uint16_t liquid_bit_y[N_LIQUID_BITS];
  graphics_context_set_stroke_color(ctx, GColorCyan);
  static bool first_pass = true; // Do we need to update all 4 coords?
  for (unsigned i = 0; i < N_LIQUID_BITS; ++i) {
    if (first_pass == true) {
      liquid_bits_bgn[i].x = 5 + rand()%130;
      liquid_bits_end[i].x =liquid_bits_bgn[i].x + (2 + rand()%3);
      liquid_bit_y[i] = 3 + rand()%50;
    }
    liquid_bits_bgn[i].y = tank_bounds.origin.y + tank_bounds.size.h - liquid_height + liquid_bit_y[i];
    liquid_bits_end[i].y = liquid_bits_bgn[i].y;
    graphics_draw_line(ctx, liquid_bits_bgn[i], liquid_bits_end[i]);
  }
  first_pass = false; // Only need update the y coord in future

  // Fill Frame
  graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_round_rect(ctx, tank_bounds, 10);
  graphics_context_set_stroke_width(ctx, 1);

  // Fill the text
  GRect text_percentage_rect = GRect(tank_bounds.origin.x + 5, 
                                     tank_bounds.origin.y + TANK_TEXT_V_OFFSET, 
                                     tank_bounds.size.w-10, 
                                     TANK_TEXT_HEIGHT);
  
  GRect text_content_rect = GRect(tank_bounds.origin.x + 5, 
                                  tank_bounds.origin.y + TANK_TEXT_V_OFFSET + TANK_TEXT_HEIGHT, 
                                  tank_bounds.size.w - 10, 
                                  TANK_TEXT_HEIGHT);


  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx,
                     s_tankContentText, 
                     *getDOSFont(), 
                     text_content_rect,
                     GTextOverflowModeWordWrap,
                     GTextAlignmentLeft,
                     NULL);
  
  if (_percentage == 100) graphics_context_set_text_color(ctx, GColorSunsetOrange);
  graphics_draw_text(ctx,
                     s_tankFullPercetText, 
                     *getDOSFont(), 
                     text_percentage_rect,
                     GTextOverflowModeWordWrap,
                     GTextAlignmentCenter,
                     NULL);

  	APP_LOG(APP_LOG_LEVEL_DEBUG, "Drew text");
}

void create_timeTank_layer(Window* parentWindow) {
  // Create the timeTank layer in the bottom 1/3 of the screen
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_tankLayer = layer_create( GRect(0, 2*(window_bounds.size.h/3), window_bounds.size.w, window_bounds.size.h/3) );
  // Add as child of the main window layer and set callback
  layer_add_child(window_layer, s_tankLayer);
  layer_set_update_proc(s_tankLayer, timeTank_update_proc); 
}

void destroy_timeTank_layer() {
  layer_destroy(s_tankLayer);
}