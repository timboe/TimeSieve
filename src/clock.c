#include <pebble.h>
#include <clock.h>
#include <resources.h> 

static Layer* s_clockLayer;

static void clock_update_proc(Layer *this_layer, GContext *ctx) {
  GRect tank_bounds = layer_get_bounds(this_layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, tank_bounds, 0, GCornersAll);
  // Fill back

  // Do the clock
  static char s_timeBuffer[] = "00:00";
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
 
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    strftime(s_timeBuffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(s_timeBuffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  int trX = tank_bounds.origin.x + 3;
  int trY = tank_bounds.origin.y+6;
  int trW = tank_bounds.size.w-6;
  int trH = 46;

  int s = 2;
  GRect timeRect  = GRect(trX,   trY,   trW, trH);
  GRect timeRectR = GRect(trX+s, trY,   trW, trH);
  GRect timeRectD = GRect(trX,   trY-s, trW, trH);
  GRect timeRectU = GRect(trX,   trY+s,   trW, trH);
  GRect timeRectL = GRect(trX-s, trY,   trW, trH);

  GRect timeRectDA = GRect(trX+s, trY+s, trW, trH);
  GRect timeRectDB = GRect(trX+s, trY-s, trW, trH);
  GRect timeRectDC = GRect(trX-s, trY+s, trW, trH);
  GRect timeRectDD = GRect(trX-s, trY-s, trW, trH);




  graphics_context_set_text_color(ctx, GColorLiberty); // Darkest
  graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectDA, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectDB, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectDC, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectDD, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorCeleste); // Light
  graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectU, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorVividCerulean); // Medium
  graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectD, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorCadetBlue); // Muddy
  graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectL, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorElectricBlue); // Light
  graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectR, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorLiberty); // Darkest
  graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);



  // graphics_context_set_text_color(ctx, GColorLiberty);
  // graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectDA, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  // graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectDB, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  // graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectDC, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  // graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectDD, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // graphics_context_set_text_color(ctx, GColorTiffanyBlue);
  // graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectU, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // graphics_context_set_text_color(ctx, GColorVividCerulean);
  // graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectR, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // graphics_context_set_text_color(ctx, GColorVeryLightBlue);
  // graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectD, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // graphics_context_set_text_color(ctx, GColorElectricBlue);
  // graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRectL, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // graphics_context_set_text_color(ctx, GColorLiberty);
  // graphics_draw_text(ctx, s_timeBuffer, *getLargeDOSFont(), timeRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
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