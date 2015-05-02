#include <pebble.h>
#include <clock.h>
#include <resources.h> 
#include <constants.h> 

static Layer* s_clockLayer;
static char s_timeBuffer[CLOCK_TEXT_SIZE];

static GPoint s_spoogelet[N_SPOOGELET];
static int16_t s_spoogeletVx[N_SPOOGELET];
static int16_t s_spoogeletVy[N_SPOOGELET];
static GPoint s_attractor;

static uint8_t s_tickCount = 0;

void animCallback(void *data) {
  spoogeAnimation();
}

bool spoogeAnimation() {


  // Animation reset
  if (s_tickCount == 0) {
    for (unsigned i = 0; i < N_SPOOGELET; ++i) {
      s_spoogelet[i].x = (7*WIN_SIZE_X/16 + (rand() % WIN_SIZE_X/8)) * SUB_PIXEL;
      s_spoogelet[i].y = (20 + (rand() % 10)) * SUB_PIXEL;

      int32_t angle = rand() % TRIG_MAX_ANGLE;
      int32_t v = ANIM_MIN_V + (rand() % (ANIM_MAX_V-ANIM_MIN_V));
      s_spoogeletVx[i] =  sin_lookup(angle)* v / TRIG_MAX_RATIO;
      s_spoogeletVy[i] = -cos_lookup(angle)* v / TRIG_MAX_RATIO;
    }
    s_attractor.x = 130;//130
    s_attractor.y = 60;//60
  }



  int16_t strength =0;
  if (s_tickCount > 24) strength = s_tickCount;
  //int16_t progress = ANIM_FRAMES / 48;
  //if (progress) strength = s_tickCount / progress;

  for (unsigned i = 0; i < N_SPOOGELET; ++i) {

    s_spoogelet[i].x += s_spoogeletVx[i];
    s_spoogelet[i].y += s_spoogeletVy[i];


    int16_t xOff = ((s_attractor.x * SUB_PIXEL) - s_spoogelet[i].x);
    int16_t yOff = ((s_attractor.y * SUB_PIXEL) - s_spoogelet[i].y);

    // I go from 0 to 50, at 50 i need to set the offset so that they will be on top off each other
    // before that, it should nudge closer

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "B4 SCALE str %i xOff %i yOff %i", (int)strength, (int)xOff, (int)yOff);


    xOff = (xOff * strength) / 100;
    yOff = (yOff * strength) / 100; // nominally FPS*2 = 50

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "AF SCALE str %i xOff %i yOff %i", (int)strength, (int)xOff/SUB_PIXEL, (int)yOff/SUB_PIXEL);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "AB SCALE str %i xOff %i yOff %i", (int)strength, (int)xOff, (int)yOff);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, " ");




    s_spoogelet[i].x += xOff;
    s_spoogelet[i].y += yOff;

    //s_spoogeletVx[i] += xOff * strength * SUB_PIXEL / 1000;
    //s_spoogeletVy[i] += yOff * strength * SUB_PIXEL / 1000;

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "str %i xOff %i yOff %i", (int)strength, (int)xOff/SUB_PIXEL, (int)yOff/SUB_PIXEL);

    //if ( !(xOff < 6 && xOff > -6 && yOff < 6 && yOff > -6) ) {

   // }

  }

  // if refeshing screen
  //if (s_tickCount % ANIM_TICK_PER_FRAME == 0) {
    layer_mark_dirty(s_clockLayer); // Redraw
    ++s_tickCount;
  //}


  if (s_tickCount == ANIM_FRAMES) {
    s_tickCount = 0;
    // animation done
    return false;
  } else {
    app_timer_register(ANIM_DELAY, animCallback, NULL);
    //register for another call
    return true;
  }
}

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

  graphics_context_set_stroke_color(ctx, GColorBlack);
  for (unsigned i = 0; i < N_SPOOGELET; ++i) {
    const uint8_t r = 1+rand()%3;
    const GPoint p = GPoint(s_spoogelet[i].x / SUB_PIXEL, s_spoogelet[i].y / SUB_PIXEL);
    if (i%2==0) graphics_context_set_fill_color(ctx, GColorPictonBlue);
    else        graphics_context_set_fill_color(ctx, GColorMintGreen);
    graphics_fill_circle(ctx, p, r);
    graphics_draw_circle(ctx, p, r);
  }
  graphics_context_set_fill_color(ctx, GColorGreen);
  graphics_fill_circle(ctx, s_attractor, 3);

}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_clockLayer);
}

void create_clock_layer(Window* parentWindow) {
  // Create the clock layer in the top 1/3 of the screen
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_clockLayer = layer_create( GRect(0, 0, window_bounds.size.w, 2*window_bounds.size.h/3) );
  // Add as child of the main window layer and set callback
  layer_add_child(window_layer, s_clockLayer);
  layer_set_update_proc(s_clockLayer, clock_update_proc); 
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  layer_set_clips(s_clockLayer, false);

}


void destroy_clock_layer() {
  layer_destroy(s_clockLayer);
}