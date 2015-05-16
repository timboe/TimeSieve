#include <pebble.h>
#include <clock.h>
#include <resources.h>
#include <constants.h>
#include <palette.h>
#include <persistence.h>

static Layer* s_clockLayer;
static char s_timeBuffer[CLOCK_TEXT_SIZE];
static BatteryChargeState s_battery;

static GPoint s_spoogelet[N_SPOOGELET];
static int16_t s_spoogeletVx[N_SPOOGELET];
static int16_t s_spoogeletVy[N_SPOOGELET];
static GPoint s_attractor;

static uint8_t s_clockTickCount = 0;

static uint8_t s_clockPixelOffset = 2;
static bool s_flashMainFace = false;

#define CLOCK_OFFSET 16

void setClockPixelOffset(uint8_t offset) {
  s_clockPixelOffset = offset;
}

void updateClockLayer() {
  if (s_clockLayer) layer_mark_dirty(s_clockLayer);
}

void clockAnimReset(TimeUnits units_changed) {
  s_clockTickCount = 0;
  for (unsigned i = 0; i < N_SPOOGELET; ++i) {
    s_spoogelet[i].x = (7*WIN_SIZE_X/16 + (rand() % WIN_SIZE_X/8)) * SUB_PIXEL;
    s_spoogelet[i].y = (30 + (rand() % 10)) * SUB_PIXEL;

    int32_t angle = rand() % TRIG_MAX_ANGLE;
    int32_t v = ANIM_MIN_V + (rand() % (ANIM_MAX_V-ANIM_MIN_V));
    s_spoogeletVx[i] =  sin_lookup(angle) * v / TRIG_MAX_RATIO;
    s_spoogeletVy[i] = -cos_lookup(angle) * v / TRIG_MAX_RATIO;
  }
  s_attractor.x = 110;//130
  s_attractor.y = 75;//60
}

bool clockAnimCallback(TimeUnits units_changed) {

  int16_t strength = 0;
  if (s_clockTickCount > 24) strength = s_clockTickCount;

  // Hour+ specific animation
  if ((units_changed & HOUR_UNIT) > 0 && s_clockTickCount % 4 == 0) {
    s_flashMainFace = !s_flashMainFace;
  }

  // Month+ specific animation
  if ((units_changed & MONTH_UNIT) > 0 && s_clockTickCount % 6 == 0) {
    colourOverride( rand() % PALETTE_MAX );
  }

  if ((units_changed & YEAR_UNIT) > 0 && s_clockTickCount % 8 == 0) {
    uint8_t bgColourOverride = rand() % PALETTE_MAX;
    while ( bgColourOverride == getColour() ) bgColourOverride = rand() % PALETTE_MAX;
    window_set_background_color( layer_get_window(s_clockLayer), getBGFlashColour(bgColourOverride) );
  }

  for (unsigned i = 0; i < N_SPOOGELET; ++i) {

    s_spoogelet[i].x += s_spoogeletVx[i];
    s_spoogelet[i].y += s_spoogeletVy[i];

    int16_t xOff = ((s_attractor.x * SUB_PIXEL) - s_spoogelet[i].x);
    int16_t yOff = ((s_attractor.y * SUB_PIXEL) - s_spoogelet[i].y);

    // I go from 0 to 50, at 50 i need to set the offset so that they will be on top off each other
    // before that, it should nudge closer

    xOff = (xOff * strength) / 100;
    yOff = (yOff * strength) / 100; // nominally FPS*2 = 50

    s_spoogelet[i].x += xOff;
    s_spoogelet[i].y += yOff;
  }

  updateClockLayer(); // Redraw

  if (++s_clockTickCount == ANIM_FRAMES) {
    s_flashMainFace = false;
    colourOverride( -1 );
    window_set_background_color( layer_get_window(s_clockLayer), GColorBlack );
    return false;
  } else {
    return true; // Request more frames
  }
}

void drawClock(GContext *ctx, GRect loc) {

  GFont* f = getClockFont();

  // corners
  graphics_context_set_text_color(ctx, getTextColourL());
  loc.origin.x -= s_clockPixelOffset; // CL
  loc.origin.y += s_clockPixelOffset; // UL
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, getTextColourC());
  loc.origin.x += s_clockPixelOffset; // CU
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  loc.origin.x += s_clockPixelOffset; // RU
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);


  graphics_context_set_text_color(ctx, getTextColourR());
  loc.origin.y -= s_clockPixelOffset; // CR
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  loc.origin.y -= s_clockPixelOffset; // DR
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);


  graphics_context_set_text_color(ctx, getTextColourD());
  loc.origin.x -= s_clockPixelOffset; // DC
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  loc.origin.x -= s_clockPixelOffset; // DR
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, getTextColourL());
  loc.origin.y += s_clockPixelOffset; // CR
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // main
  if (s_flashMainFace) graphics_context_set_text_color(ctx, getTextColourU());
  else graphics_context_set_text_color(ctx, getTextColourC());
  loc.origin.x += s_clockPixelOffset; // O
  graphics_draw_text(ctx, s_timeBuffer, *f, loc, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

void updateTimeBuffer() {
  // Do the clock
  time_t temp = time(NULL);
  struct tm *tickTime = localtime(&temp);
  // Write the current hours and minutes and maybe secods into the buffer
  if (getUserOpt(OPT_SHOW_SECONDS) == true) {
    if(clock_is_24h_style() == true) {
      strftime(s_timeBuffer, CLOCK_TEXT_SIZE*sizeof(char), "%H:%M:%S", tickTime);
    } else {
      strftime(s_timeBuffer, CLOCK_TEXT_SIZE*sizeof(char), "%I:%M:%S", tickTime);
    }
  } else {
    if(clock_is_24h_style() == true) {
      strftime(s_timeBuffer, CLOCK_TEXT_SIZE*sizeof(char), "%H:%M", tickTime);
    } else {
      strftime(s_timeBuffer, CLOCK_TEXT_SIZE*sizeof(char), "%I:%M", tickTime);
    }
  }
  s_battery = battery_state_service_peek();
}

static void clock_update_proc(Layer *this_layer, GContext *ctx) {
  GRect tank_bounds = layer_get_bounds(this_layer);

  // BATTERY
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, getLiquidTimeHighlightColour());
  graphics_draw_rect(ctx, GRect(115,5,18,7));
  graphics_draw_rect(ctx, GRect(133,7,2,3));
  graphics_fill_rect(ctx, GRect(117,7,s_battery.charge_percent/7,3), 0, GCornersAll); // 100%=14 pixels


  GRect timeRect = GRect(tank_bounds.origin.x, tank_bounds.origin.y + CLOCK_OFFSET, tank_bounds.size.w, tank_bounds.size.h - CLOCK_OFFSET);
  drawClock(ctx, timeRect);

  if (s_clockTickCount == 0) return; // No animation in progress

  graphics_context_set_stroke_color(ctx, GColorBlack);
  for (unsigned i = 0; i < N_SPOOGELET; ++i) {
    const GPoint p = GPoint(s_spoogelet[i].x / SUB_PIXEL, s_spoogelet[i].y / SUB_PIXEL);
    if ((p.x - s_attractor.x) < 8 && (p.x - s_attractor.x) > -8 && (p.y - s_attractor.y) < 8 && (p.y - s_attractor.y) > -8) continue; // Too close
    const uint8_t r = 1+rand()%3;
    if (i%2==0) graphics_context_set_fill_color(ctx, getSpoogicalColourA());
    else        graphics_context_set_fill_color(ctx, getSpoogicalColourB());
    graphics_fill_circle(ctx, p, r);
    graphics_draw_circle(ctx, p, r);
  }
  graphics_context_set_fill_color(ctx, GColorGreen);
  graphics_fill_circle(ctx, s_attractor, 3);

}

void create_clock_layer(Window* parentWindow) {
  // Create the clock layer in the top 1/3 of the screen
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_clockLayer = layer_create( GRect(0, 0, window_bounds.size.w, 2*window_bounds.size.h/3) );
  // Add as child of the main window layer and set callback
  layer_add_child(window_layer, s_clockLayer);
  layer_set_update_proc(s_clockLayer, clock_update_proc);
  layer_set_clips(s_clockLayer, false);

}


void destroy_clock_layer() {
  layer_destroy(s_clockLayer);
  s_clockLayer = 0;
}
