#include <pebble.h>
#include "resources.h"
#include "persistence.h"

static GFont s_perfectDOSFont;
static GFont s_font1;
static GFont s_font2;
static GFont s_font3;
static GFont s_font4;
static GFont s_font5;

GFont* getDOSFont() {
  return &s_perfectDOSFont;
}

GFont* getClockFont() {
  switch (getUserTypeSetting()) {
    case FONT_1: return &s_font1;
    case FONT_2: return &s_font2;
    case FONT_3: return &s_font3;
    case FONT_4: return &s_font4;
    case FONT_5: return &s_font5;
    default: return &s_font1;
  }
}

void init_resources() {
  s_perfectDOSFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PERFECT_DOS_21));
  s_font1          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_31));
  s_font2          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_B_29));
  s_font3          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_C_29));
  s_font4          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_D_31));
  s_font5          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_E_31));
}

void destroy_resources() {
  fonts_unload_custom_font(s_perfectDOSFont);
  fonts_unload_custom_font(s_font1);
  fonts_unload_custom_font(s_font2);
  fonts_unload_custom_font(s_font3);
  fonts_unload_custom_font(s_font4);
  fonts_unload_custom_font(s_font5); 
}