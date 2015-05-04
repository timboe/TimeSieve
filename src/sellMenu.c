#include <pebble.h>
#include "sellMenu.h"
#include "constants.h"
#include "persistence.h"
#include "timeStore.h"


// #define CHEVO_CONTEXT_ID 0
// #define UNIQUE_CONTEXT_ID 1

// #define STAT_SECTION_ID 0
// #define CHEVO_SECTION_ID 1
// #define SETTINGS_SECTION_ID 2
  
// #define NUM_SETTINGS_MENU_SECTIONS 3
// //
// #define NUM_STAT_ROWS 3
// #define NUM_CHEVO_ROWS 2
// #define NUM_SETTINGS_ROWS 9

// #define SETTINGS_CELL_HEIGHT 57

static int8_t s_sellSections[SELLABLE_CATEGORIES] = {-1};

static MenuLayer* s_sell_layer;  

static char tempBuffer[TEXT_BUFFER_SIZE];

/// 
/// SELL WINDOW CALLBACKS
///

/**
 * One (empty) section at the top for the heade, plus one section for each category of item the player ownes
 */
static uint16_t sell_get_num_sections_callback(MenuLayer *menu_layer, void *data) { 
  uint16_t sections = 0;
  for (uint16_t i = 0; i < SELLABLE_CATEGORIES; ++i) {
    if (getUserTotalItems(i) > 0) s_sellSections[sections] = i;
  }
  if (sections == 0) {
    s_sellSections[0] = -1;
    return 1; // Generic title
  }
  return sections;
}

static uint16_t sell_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { 
  if (s_sellSections[section_index] == -1) return 1; // If no items to sell
  return getUserItemTypes( s_sellSections[section_index] );
}

static int16_t sell_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { 
  switch (section_index) {
    case 0: return 2*MENU_CELL_BASIC_HEADER_HEIGHT;
    default: return MENU_CELL_BASIC_HEADER_HEIGHT;
  }
}

static void sell_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  const GSize size = layer_get_frame(cell_layer).size;
  GRect top = GRect(0, 0, size.w, MENU_CELL_BASIC_HEADER_HEIGHT);
  GRect bot = GRect(0, MENU_CELL_BASIC_HEADER_HEIGHT, size.w, MENU_CELL_BASIC_HEADER_HEIGHT);

  if (section_index == 0) {
    graphics_draw_text(ctx, "SELL Items", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), top, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    if (s_sellSections[0] == -1) {
      graphics_draw_text(ctx, "You have no items!", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), bot, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      return;
    }
    top = bot;
  }

  const int8_t category = s_sellSections[section_index];
  if (category == COMMON_ID) graphics_draw_text(ctx, "COMMON Treasures", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), top, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  else if (category == MAGIC_ID) graphics_draw_text(ctx, "MAGIC Treasures", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), top, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  else if (category == RARE_ID) graphics_draw_text(ctx, "RARE Treasures", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), top, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  else if (category == EPIC_ID) graphics_draw_text(ctx, "EPIC Treasures", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), top, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  // const GSize size = layer_get_frame(cell_layer).size;
  // if (section_index == STAT_SECTION_ID) {
  //   strcpy(tempBuffer, "STATISTICS");
  //   graphics_context_set_fill_color(ctx, MENU_BACK_YELLOW_ODD);
  // } else if (section_index == CHEVO_SECTION_ID) {
  //   strcpy(tempBuffer, "ACHIEVEMENTS");
  //   graphics_context_set_fill_color(ctx, MENU_BACK_GREEN_ODD);
  // } else if (section_index == SETTINGS_SECTION_ID) {
  //   strcpy(tempBuffer, "SETTINGS");
  //   graphics_context_set_fill_color(ctx, MENU_BACK_BLUE_ODD);
  // }
  // graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);
  // menu_cell_basic_header_draw(ctx, cell_layer, tempBuffer);
  // graphics_context_set_stroke_color(ctx, GColorBlack);
  // graphics_draw_line(ctx, GPoint(0,0), GPoint(size.w, 0) );
}

static int16_t sell_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) { 
  // switch(cell_index->section) {
  //   // case STAT_SECTION_ID: return SETTINGS_CELL_HEIGHT; 
  //   // case CHEVO_SECTION_ID: return 32;
  //   // case SETTINGS_SECTION_ID: return 44;
  //   default: return 0;
  // }
  return 32;
}

static void sell_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  const GSize size = layer_get_frame(cell_layer).size;
  const uint16_t section = cell_index->section;
  const uint16_t row = cell_index->row;
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);
  
  // // Text colours
  // if (selected) graphics_context_set_text_color(ctx, GColorWhite);
  // else graphics_context_set_text_color(ctx, GColorBlack);

  // // Back color
  // GColor backColor;
  // // if      (section == STAT_SECTION_ID && selected   ) backColor = MENU_BACK_YELLOW_SELECT;
  // // else if (section == STAT_SECTION_ID && row%2 == 0 ) backColor = MENU_BACK_YELLOW_EVEN;
  // // else if (section == STAT_SECTION_ID               ) backColor = MENU_BACK_YELLOW_ODD;
  // // else if (section == CHEVO_SECTION_ID && selected   ) backColor = MENU_BACK_GREEN_SELECT;
  // // else if (section == CHEVO_SECTION_ID && row%2 == 0 ) backColor = MENU_BACK_GREEN_EVEN;
  // // else if (section == CHEVO_SECTION_ID               ) backColor = MENU_BACK_GREEN_ODD;
  // // else if (section == SETTINGS_SECTION_ID && selected   ) backColor = MENU_BACK_BLUE_SELECT;
  // // else if (section == SETTINGS_SECTION_ID && row%2 == 0 ) backColor = MENU_BACK_BLUE_EVEN;
  // // else if (section == SETTINGS_SECTION_ID               ) backColor = MENU_BACK_BLUE_ODD;
  // graphics_context_set_fill_color(ctx, backColor);
  // graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);

  // GRect ttlTextRect = GRect(0, 0,  size.w, size.h);
  // GRect topTextRect = GRect(0, 22, size.w, size.h-22);
  // GRect botTextRect = GRect(0, 33, size.w, size.h-33);

  // static char titleText[TEXT_BUFFER_SIZE];
  // static char subText1[TEXT_LARGE_BUFFER_SIZE];
  // static char subText2[TEXT_BUFFER_SIZE];
  // strcpy(titleText, "");
  // strcpy(subText1, "");
  // strcpy(subText2, "");

  // // ...

  // graphics_draw_text(ctx, titleText, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  // graphics_draw_text(ctx, subText1, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  // graphics_draw_text(ctx, subText2, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  // graphics_context_set_stroke_color(ctx, GColorBlack);
  // graphics_draw_line(ctx, GPoint(0,0), GPoint(size.w, 0) );
}

static void sell_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  const uint16_t section = cell_index->section;
  const uint16_t row = cell_index->row;

  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

/// 
/// SETUP
///


void sell_window_load(Window* parentWindow) {
  APP_LOG(APP_LOG_LEVEL_DEBUG,"SELL WIN LOAD");

  // Now we prepare to initialize the menu layer
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  s_sell_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_sell_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = sell_get_num_sections_callback,
    .get_num_rows = sell_get_num_rows_callback,
    .get_cell_height = sell_get_cell_height_callback,
    .get_header_height = sell_get_header_height_callback,
    .draw_header = sell_draw_header_callback,
    .draw_row = sell_draw_row_callback,
    .select_click = sell_select_callback,
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_sell_layer, parentWindow);
  layer_add_child(window_layer, menu_layer_get_layer(s_sell_layer));


}

void sell_window_unload() {
  APP_LOG(APP_LOG_LEVEL_DEBUG,"SELL WIN DESTROY");
  menu_layer_destroy(s_sell_layer);
}