#include "notify.h"
#include "constants.h"

static Layer* s_notifyLayer = NULL;
static AppTimer* s_notifyTimer = NULL;

static GColor s_notifyColor;
const char* s_txtA;
const char* s_txtB;
const char* s_txtC;

// Notify popup
static void notifyUpdateProc(Layer *this_layer, GContext *ctx) {
  if (s_notifyTimer == NULL) return; // Not being shown
  GRect b = layer_get_bounds(this_layer);
  // Outer box
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, b, 13, (GCornerTopLeft|GCornerBottomRight));
  graphics_context_set_fill_color(ctx, s_notifyColor);
  graphics_fill_rect(ctx, GRect(b.origin.x+2, b.origin.y+2, b.size.w-4, b.size.h-4), 13, (GCornerTopLeft|GCornerBottomRight));
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(b.origin.x+4, b.origin.y+4, b.size.w-8, b.size.h-8), 13, (GCornerTopLeft|GCornerBottomRight));
  graphics_context_set_text_color(ctx, GColorBlack);

  graphics_draw_text(ctx, s_txtA, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(0,4,b.size.w,30), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, s_txtB, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(0,13,b.size.w,30), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, s_txtC, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(0,38,b.size.w,30), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

void removeNotify(void* data) {
  s_notifyTimer = NULL;
  layer_mark_dirty(s_notifyLayer);
}

void showNotify(GColor highlight, const char* a, const char* b, const char* c) {
  if (s_notifyTimer) app_timer_cancel(s_notifyTimer);
  s_notifyTimer = app_timer_register(NOTIFY_DISPLAY_TIME, removeNotify, NULL);
  layer_mark_dirty(s_notifyLayer);
  s_notifyColor = highlight;
  s_txtA = a;
  s_txtB = b;
  s_txtC = c;
}

Layer* getNotifyLayer() {
  // Notify layer goes on top, shows sold items
  s_notifyLayer = layer_create( GRect(0, 0, WIN_SIZE_X, 60) );
  layer_set_update_proc(s_notifyLayer, notifyUpdateProc);
  s_notifyTimer = NULL;
  return s_notifyLayer;
}

void destroyNotifyLayer() {
  layer_destroy(s_notifyLayer);
  s_notifyLayer = 0;
}
