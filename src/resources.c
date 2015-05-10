#include <pebble.h>
#include "resources.h"
#include "persistence.h"
#include "constants.h"

static GFont s_perfectDOSFont;
static GFont s_font1;
//static GFont s_font2;
//static GFont s_font3;
//static GFont s_font4;
//static GFont s_font5;

static GBitmap* s_commonItem[MAX_TREASURES];
static GBitmap* s_magicItem[MAX_TREASURES];
static GBitmap* s_rareItem[MAX_TREASURES];
static GBitmap* s_epicItem[MAX_TREASURES];
static GBitmap* s_legendaryItem[MAX_UNIQUE];

GFont* getDOSFont() {
  return &s_perfectDOSFont;
}

GFont* getClockFont() {
  return &s_font1;
  //switch (getUserTypeSetting()) {
  //  case FONT_1: return &s_font1;
  //  case FONT_2: return &s_font2;
  //  case FONT_3: return &s_font3;
  //  case FONT_4: return &s_font4;
  //  case FONT_5: return &s_font5;
  //  default: return &s_font1;
  //}
}

void init_resources() {
  s_perfectDOSFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PERFECT_DOS_21));
  s_font1          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_31));
  //s_font2          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_B_29));
  //s_font3          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_C_29));
  //s_font4          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_D_31));
  //s_font5          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_E_31));
  s_commonItem[0] = gbitmap_create_with_resource(RESOURCE_ID_COMMON_0);
  s_commonItem[1] = gbitmap_create_with_resource(RESOURCE_ID_COMMON_1);
  s_commonItem[2] = gbitmap_create_with_resource(RESOURCE_ID_COMMON_2);
  s_commonItem[3] = gbitmap_create_with_resource(RESOURCE_ID_COMMON_3);
  //
  s_magicItem[0] = gbitmap_create_with_resource(RESOURCE_ID_MAGIC_0);
  s_magicItem[1] = gbitmap_create_with_resource(RESOURCE_ID_MAGIC_1);
  s_magicItem[2] = gbitmap_create_with_resource(RESOURCE_ID_MAGIC_2);
  s_magicItem[3] = gbitmap_create_with_resource(RESOURCE_ID_MAGIC_3);
  //
  s_rareItem[0] = gbitmap_create_with_resource(RESOURCE_ID_RARE_0);
  s_rareItem[1] = gbitmap_create_with_resource(RESOURCE_ID_RARE_1);
  s_rareItem[2] = gbitmap_create_with_resource(RESOURCE_ID_RARE_2);
  s_rareItem[3] = gbitmap_create_with_resource(RESOURCE_ID_RARE_3);
  //
  s_epicItem[0] = gbitmap_create_with_resource(RESOURCE_ID_EPIC_0);
  s_epicItem[1] = gbitmap_create_with_resource(RESOURCE_ID_EPIC_1);
  s_epicItem[2] = gbitmap_create_with_resource(RESOURCE_ID_EPIC_2);
  s_epicItem[3] = gbitmap_create_with_resource(RESOURCE_ID_EPIC_3); 
  //
  s_legendaryItem[0] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_0);
  s_legendaryItem[1] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_1);  
  s_legendaryItem[2] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_2);
  s_legendaryItem[3] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_3);
  s_legendaryItem[4] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_4);
  s_legendaryItem[5] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_5);
  s_legendaryItem[6] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_6);
  s_legendaryItem[7] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_7);
  s_legendaryItem[8] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_8);
  s_legendaryItem[9] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_9);
  s_legendaryItem[10] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_10);
  s_legendaryItem[11] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_11);
  s_legendaryItem[12] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_12);
  s_legendaryItem[13] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_13);
  s_legendaryItem[14] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_14);
  s_legendaryItem[15] = gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_15);
}

void destroy_resources() {
  fonts_unload_custom_font(s_perfectDOSFont);
  fonts_unload_custom_font(s_font1);
  //fonts_unload_custom_font(s_font2);
  //fonts_unload_custom_font(s_font3);
  //fonts_unload_custom_font(s_font4);
  //fonts_unload_custom_font(s_font5); 
  for (uint8_t i=0; i < MAX_UNIQUE; ++i) {
    gbitmap_destroy( s_legendaryItem[i] );
    if (i >= MAX_TREASURES) continue;
    gbitmap_destroy( s_commonItem[i] );
    gbitmap_destroy( s_magicItem[i] );
    gbitmap_destroy( s_rareItem[i] );
    gbitmap_destroy( s_epicItem[i] );
  }

}

GBitmap* getItemImage(uint8_t treasureID, uint8_t itemID) {
  switch (treasureID) {
    case COMMON_ID: return s_commonItem[itemID];
    case MAGIC_ID: return s_magicItem[itemID];
    case RARE_ID: return s_rareItem[itemID];
    case EPIC_ID: return s_epicItem[itemID];
    case LEGENDARY_ID: return s_legendaryItem[itemID];
  }
  return NULL;
}
