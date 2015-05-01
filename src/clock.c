#include <pebble.h>
#include <clock.h>
#include <resources.h> 
#include <constants.h> 

static Layer* s_clockLayer;
static char s_timeBuffer[CLOCK_TEXT_SIZE];

static GPoint s_spoogelet[N_SPOOGELET];

void drawClock(GContext *ctx, GRect loc) {

  GFont* f = getClockFont();

  // corners
  graphics_context_set_text_color(ctx, GColorCadetBlue); // Muddy (L)
  loc.origin.x -= CLOCK_PIX_OFFSET; // CL
  loc.origin.y += CLOCK_PIX_OFFSET; // UL
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorCeleste); // Light U
  loc.origin.x += CLOCK_PIX_OFFSET; // CU
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  loc.origin.x += CLOCK_PIX_OFFSET; // RU
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);


  graphics_context_set_text_color(ctx, GColorElectricBlue); // Light R
  loc.origin.y -= CLOCK_PIX_OFFSET; // CR
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  loc.origin.y -= CLOCK_PIX_OFFSET; // DR
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);


  graphics_context_set_text_color(ctx, GColorVividCerulean); // Medium D
  loc.origin.x -= CLOCK_PIX_OFFSET; // DC
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  loc.origin.x -= CLOCK_PIX_OFFSET; // DR
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorCadetBlue); // Muddy (L)
  loc.origin.y += CLOCK_PIX_OFFSET; // CR
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // main
  graphics_context_set_text_color(ctx, GColorLiberty); // Darkest
  loc.origin.x += CLOCK_PIX_OFFSET; // O
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void clock_update_proc(Layer *this_layer, GContext *ctx) {
  GRect tank_bounds = layer_get_bounds(this_layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, tank_bounds, 0, GCornersAll);
  // Fill back

  // Do the clock
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
 
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    strftime(s_timeBuffer, CLOCK_TEXT_SIZE*sizeof(char), "%H:%M:%S", tick_time);
  } else {
    strftime(s_timeBuffer, CLOCK_TEXT_SIZE*sizeof(char), "%I:%M:%S", tick_time);
  }

  GRect timeRect = GRect(tank_bounds.origin.x, tank_bounds.origin.y+6, tank_bounds.size.w, 46);
  drawClock(ctx, timeRect);

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

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_clockLayer);
}

void create_clock_layer(Window* parentWindow) {
  // Create the clock layer in the top 1/3 of the screen
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_clockLayer = layer_create( GRect(0, 0, window_bounds.size.w, window_bounds.size.h/3) );
  // Add as child of the main window layer and set callback
  layer_add_child(window_layer, s_clockLayer);
  layer_set_update_proc(s_clockLayer, clock_update_proc); 
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);


}


void destroy_clock_layer() {
  layer_destroy(s_clockLayer);
}