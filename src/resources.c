#include <pebble.h>
#include "resources.h"

static GFont s_perfectDOSFont;

GFont* getDOSFont() {
  return &s_perfectDOSFont;
}

void init_resources() {
  s_perfectDOSFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PERFECT_DOS_21));
}

void destroy_resources() {
  fonts_unload_custom_font(s_perfectDOSFont);
}