#include <pebble.h>
#include "resources.h"

static GFont s_perfectDOSFont;
static GFont s_thin;

GFont* getDOSFont() {
  return &s_perfectDOSFont;
}

GFont* getClockFont() {
  return &s_thin;
}

void init_resources() {
  s_perfectDOSFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PERFECT_DOS_21));
  s_thin           = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BOWMAN_31));
}

void destroy_resources() {
  fonts_unload_custom_font(s_perfectDOSFont);
  fonts_unload_custom_font(s_thin);
}