#include <pebble.h>
#include "settingsMenu.h"
#include "constants.h"
#include "persistence.h"
#include "timeStore.h"

#define CHEVO_CONTEXT_ID 0
#define UNIQUE_CONTEXT_ID 1

#define STAT_SECTION_ID 0
#define CHEVO_SECTION_ID 1
#define SETTINGS_SECTION_ID 2
  
#define NUM_SETTINGS_MENU_SECTIONS 3
#define NUM_STAT_ROWS 2
#define NUM_CHEVO_ROWS 2
#define NUM_SETTINGS_ROWS 4 

static MenuLayer* s_settings_layer;  
static MenuLayer* s_chevo_layer;  
static MenuLayer* s_unique_layer;

static Window* s_chevo_window;
static Window* s_unique_window;

static int s_chevo_context = CHEVO_CONTEXT_ID;
static int s_unique_context = UNIQUE_CONTEXT_ID;

static char tempBuffer[TEXT_BUFFER_SIZE];

/// 
/// SETTINGS WINDOW CALLBACKS
///

static uint16_t settings_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return NUM_SETTINGS_MENU_SECTIONS; }

static uint16_t settings_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { 
  switch (section_index) {
  	case STAT_SECTION_ID: return NUM_STAT_ROWS;
  	case CHEVO_SECTION_ID: return NUM_CHEVO_ROWS;
  	case SETTINGS_SECTION_ID: return NUM_SETTINGS_ROWS;
  	default: return 0;
  }
}

static int16_t settings_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return MENU_CELL_BASIC_HEADER_HEIGHT; }

static void settings_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  const GSize size = layer_get_frame(cell_layer).size;
  if (section_index == STAT_SECTION_ID) {
    strcpy(tempBuffer, "STATISTICS");
    graphics_context_set_fill_color(ctx, MENU_BACK_YELLOW_EVEN);
  } else if (section_index == CHEVO_SECTION_ID) {
    strcpy(tempBuffer, "ACHIEVEMENTS");
    graphics_context_set_fill_color(ctx, MENU_BACK_BLUE_EVEN);
  } else if (section_index == SETTINGS_SECTION_ID) {
    strcpy(tempBuffer, "SETTINGS");
    graphics_context_set_fill_color(ctx, MENU_BACK_GREEN_EVEN);
  }
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);
  menu_cell_basic_header_draw(ctx, cell_layer, tempBuffer);
}

static void settings_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  const GSize size = layer_get_frame(cell_layer).size;
  const uint16_t section = cell_index->section;
  const uint16_t row = cell_index->row;
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);
  
  // Text colours
  if (selected) graphics_context_set_text_color(ctx, GColorWhite);
  else graphics_context_set_text_color(ctx, GColorBlack);

  // Back color
  GColor backColor;
  if      (section == STAT_SECTION_ID && selected   ) backColor = MENU_BACK_YELLOW_SELECT;
  else if (section == STAT_SECTION_ID && row%2 == 0 ) backColor = MENU_BACK_YELLOW_EVEN;
  else if (section == STAT_SECTION_ID               ) backColor = MENU_BACK_YELLOW_ODD;
  else if (section == CHEVO_SECTION_ID && selected   ) backColor = MENU_BACK_GREEN_SELECT;
  else if (section == CHEVO_SECTION_ID && row%2 == 0 ) backColor = MENU_BACK_GREEN_EVEN;
  else if (section == CHEVO_SECTION_ID               ) backColor = MENU_BACK_GREEN_ODD;
  else if (section == SETTINGS_SECTION_ID && selected   ) backColor = MENU_BACK_BLUE_SELECT;
  else if (section == SETTINGS_SECTION_ID && row%2 == 0 ) backColor = MENU_BACK_BLUE_EVEN;
  else if (section == SETTINGS_SECTION_ID               ) backColor = MENU_BACK_BLUE_ODD;
  graphics_context_set_fill_color(ctx, backColor);
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);

  // // Do not have and cannot afford, draw ???s
  // if (display == false) {
  //   //draw_dithered_rect(ctx, GRect(0, 0, size.w, size.h), GColorLightGray, GColorWhite, DITHER_10_PERCENT);
  //   GRect ttlTextRect = GRect(MENU_X_OFFSET, 0,  size.w-MENU_X_OFFSET, size.h);
  //   graphics_draw_text(ctx, "??????", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  // } else {
  //   // Now text
  //   GRect ttlTextRect = GRect(MENU_X_OFFSET, 0,  size.w-MENU_X_OFFSET, size.h);
  //   GRect topTextRect = GRect(MENU_X_OFFSET, 22, size.w-MENU_X_OFFSET, size.h-22);
  //   GRect medTextRect = GRect(MENU_X_OFFSET, 33, size.w-MENU_X_OFFSET, size.h-33);
  //   GRect botTextRect = GRect(MENU_X_OFFSET, 44, size.w-MENU_X_OFFSET, size.h-44);
    
  //   timeToString(priceNext, tempBuffer, TEXT_BUFFER_SIZE, true);
  //   static char s_header[TEXT_BUFFER_SIZE];
  //   strcpy(s_header, "COST: ");
  //   strcat(s_header, tempBuffer);

  //   static char s_owned[TEXT_BUFFER_SIZE];
  //   snprintf(s_owned, TEXT_BUFFER_SIZE, "OWNED: %i", owned);

  //   timeToString(reward, tempBuffer, TEXT_BUFFER_SIZE, true);
  //   static char s_reward[TEXT_BUFFER_SIZE];
  //   strcpy(s_reward, upgradeText);
  //   strcat(s_reward, tempBuffer);

  //   graphics_draw_text(ctx, upgradeName, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  //   graphics_draw_text(ctx, s_header, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  //   graphics_draw_text(ctx, s_owned, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), medTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  //   graphics_draw_text(ctx, s_reward, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  // }

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, GPoint(0,0), GPoint(size.w, 0) );
}

static void settings_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  // const int row = cell_index->row;
  // if (row == REFINERY_ID) { 
  //   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH REFINERY");
  //   window_stack_push(s_refinery_window, true);
  // } else if (row == TANK_ID) {
  //   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH TANK");
  //   window_stack_push(s_tank_window, true);
  // } else if (row == SIEVE_ID) {
  //   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH SIEVE");
  //   window_stack_push(s_sieve_window, true);
  // } else if (row == WATCHER_ID) {
  //   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH WATCHER");
  //   window_stack_push(s_watcher_window, true);
  // }
//   switch (cell_index->row) {
//     // This is the menu item with the cycling icon
//     case 1:
//       // Cycle the icon
//       //s_current_icon = (s_current_icon + 1) % NUM_MENU_ICONS;
//       // After changing the icon, mark the layer to have it updated
//       layer_mark_dirty(menu_layer_get_layer(menu_layer));
//       break;
//   }
}

/// 
/// REFINARY BUY WINDOW CALLBACKS
///

static uint16_t settings_sub_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return 1; }

static uint16_t settings_sub_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { 
  // const int context = *((int*)data);
  // if (context == REFINERY_ID) return N_REFINERY_UPGRADES;
  // else if (context == TANK_ID) return N_TANK_UPGRADES;
  // else if (context == SIEVE_ID) return N_SIEVE_UPGRADES;
  // else if (context == WATCHER_ID) return N_WATCHER_UPGRADES;
  return 0;
}

static int16_t settings_sub_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return 0; }

static int16_t settings_sub_menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) { return MENU_CELL_HEIGHT; }

static void settings_sub_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "HEADER DRAW %i", section_index);
//   switch (section_index) {
//   	case 
//   }
//   menu_cell_basic_header_draw(ctx, cell_layer, "");

//   const int context = *((int*)data);
//   const GSize size = layer_get_frame(cell_layer).size;
//   timeToString(getUserTime(), tempBuffer, TEXT_BUFFER_SIZE, true);
//   static char s_title[TEXT_BUFFER_SIZE];
//   static char s_header[TEXT_BUFFER_SIZE];
//   strcpy(s_header, "AVAILABLE: ");
//   strcat(s_header, tempBuffer);
//   if (context == REFINERY_ID) strcpy(s_title, "REFINARY Upgrades");
//   else if (context == TANK_ID) strcpy(s_title, "TANK Upgrades");
//   else if (context == SIEVE_ID) strcpy(s_title, "SIEVE Upgrades");
//   else if (context == WATCHER_ID) strcpy(s_title, "WATCHER Upgrades");
//   GRect topTextRect = GRect(2, 0, size.w-2, size.h);
//   GRect botTextRect = GRect(2, 14, size.w-2, size.h-14);
//   graphics_context_set_text_color(ctx, GColorBlack);
//   graphics_draw_text(ctx, s_title, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
//   graphics_draw_text(ctx, s_header, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void settings_sub_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "ROW DRAW %i", cell_index->row);

//   const int context = *((int*)data);
//   const GSize size = layer_get_frame(cell_layer).size;
//   const int row = cell_index->row;
//   const bool selected = menu_cell_layer_is_highlighted(cell_layer);
  
//   //Get details
//   const unsigned location = cell_index->row;
//   const int owned = getUserOwnsUpgrades(context, location);
//   const uint64_t priceNext = getPriceOfUpgrade(context, location);
//   const char* upgradeName = NULL;
//   uint64_t reward = 0;

//   char upgradeText[TEXT_BUFFER_SIZE];
//   if (context == REFINERY_ID) {
//     upgradeName = NAME_REFINERY[location];
//     reward = REWARD_REFINERY[location];
//     strcpy(upgradeText, "GAIN: ");
//   } else if (context == TANK_ID) {
//     upgradeName = NAME_TANK[location];
//     reward = REWARD_TANK[location];
//     strcpy(upgradeText, "SIZE: ");
//   } else if (context == SIEVE_ID) {
//     upgradeName = NAME_SIEVE[location];
//     reward = REWARD_SIEVE[location];
//     strcpy(upgradeText, "CHANCE: ");
//   } else if (context == WATCHER_ID) {
//     upgradeName = NAME_WATCHER[location];
//     reward = REWARD_WATCHER[location];
//     strcpy(upgradeText, "CHANCE: ");
//   }

//   bool display = false, canAfford = false;
//   // Can I afford this? Only display if I am more than 50% of the way to being able to afford one
//   if (owned > 0 || getUserTime() > (priceNext/2)) display = true;
//   if (getUserTime() >= priceNext) canAfford = true;

//   // Text colours
//   if (selected) graphics_context_set_text_color(ctx, GColorWhite);
//   else graphics_context_set_text_color(ctx, GColorBlack);

//   // Back color
//   GColor backColor;
//   if (selected && !display)         backColor = GColorDarkGray;
//   else if (!display)                backColor = GColorLightGray;
//   else if (selected && canAfford)   backColor = MENU_BACK_GREEN_SELECT;
//   else if (selected)                backColor = MENU_BACK_RED_SELECT;
//   else if (row%2 == 1 && canAfford) backColor = MENU_BACK_GREEN_ODD;
//   else if (canAfford)               backColor = MENU_BACK_GREEN_EVEN;
//   else if (row%2 == 1)              backColor = MENU_BACK_RED_ODD;
//   else                              backColor = MENU_BACK_RED_EVEN;
//   graphics_context_set_fill_color(ctx, backColor);
//   graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);

//   // Do not have and cannot afford, draw ???s
//   if (display == false) {
//     //draw_dithered_rect(ctx, GRect(0, 0, size.w, size.h), GColorLightGray, GColorWhite, DITHER_10_PERCENT);
//     GRect ttlTextRect = GRect(MENU_X_OFFSET, 0,  size.w-MENU_X_OFFSET, size.h);
//     graphics_draw_text(ctx, "??????", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
//   } else {
//     // Now text
//     GRect ttlTextRect = GRect(MENU_X_OFFSET, 0,  size.w-MENU_X_OFFSET, size.h);
//     GRect topTextRect = GRect(MENU_X_OFFSET, 22, size.w-MENU_X_OFFSET, size.h-22);
//     GRect medTextRect = GRect(MENU_X_OFFSET, 33, size.w-MENU_X_OFFSET, size.h-33);
//     GRect botTextRect = GRect(MENU_X_OFFSET, 44, size.w-MENU_X_OFFSET, size.h-44);
    
//     timeToString(priceNext, tempBuffer, TEXT_BUFFER_SIZE, true);
//     static char s_header[TEXT_BUFFER_SIZE];
//     strcpy(s_header, "COST: ");
//     strcat(s_header, tempBuffer);

//     static char s_owned[TEXT_BUFFER_SIZE];
//     snprintf(s_owned, TEXT_BUFFER_SIZE, "OWNED: %i", owned);

//     timeToString(reward, tempBuffer, TEXT_BUFFER_SIZE, true);
//     static char s_reward[TEXT_BUFFER_SIZE];
//     strcpy(s_reward, upgradeText);
//     strcat(s_reward, tempBuffer);

//     graphics_draw_text(ctx, upgradeName, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
//     graphics_draw_text(ctx, s_header, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
//     graphics_draw_text(ctx, s_owned, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), medTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
//     graphics_draw_text(ctx, s_reward, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
//   }

//   graphics_context_set_stroke_color(ctx, GColorBlack);
//   graphics_draw_line(ctx, GPoint(0,0), GPoint(size.w, 0) );

}

/**
 * User is trying to buy something, trigger the purchase, will only go through if it is affordable
 */
static void settings_sub_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // const int context = *((int*)data);
  // const bool result = doPurchase(context, cell_index->row);  
  // layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

/// 
/// SETUP
///

void settings_sub_window_load(Window* parentWindow) {
  // Now we prepare to initialize the menu layer
  Layer* window_layer = window_get_root_layer(parentWindow);
  const GRect bounds = layer_get_frame(window_layer);

  MenuLayer* new_layer;
  const int context = *((int*) window_get_user_data(parentWindow));
  if (context == CHEVO_CONTEXT_ID)       new_layer = s_chevo_layer;
  else if (context == UNIQUE_CONTEXT_ID) new_layer = s_unique_layer;
  // Create the menu layer
  new_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(new_layer, window_get_user_data(parentWindow), (MenuLayerCallbacks){
    .get_num_sections = settings_sub_menu_get_num_sections_callback,
    .get_num_rows = settings_sub_menu_get_num_rows_callback,
    .get_cell_height = settings_sub_menu_get_cell_height_callback,
    .get_header_height = settings_sub_menu_get_header_height_callback,
    .draw_header = settings_sub_menu_draw_header_callback,
    .draw_row = settings_sub_menu_draw_row_callback,
    .select_click = settings_sub_menu_select_callback,
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(new_layer, parentWindow);
  layer_add_child(window_layer, menu_layer_get_layer(new_layer));
}

void settings_sub_window_unload(Window* parentWindow) {
  const int context = *((int*) window_get_user_data(parentWindow));
  if (context == CHEVO_CONTEXT_ID)       menu_layer_destroy(s_chevo_layer);
  else if (context == UNIQUE_CONTEXT_ID) menu_layer_destroy(s_unique_layer);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
void createSettingsSubWin(Window** w, int* context) {
  *w = window_create();
  window_set_user_data(*w, context);
  window_set_background_color(*w, MENU_BACK_BLUE_ODD);
  window_set_window_handlers(*w, (WindowHandlers) {
    .load = settings_sub_window_load,
    .unload = settings_sub_window_unload
  }); 
}

void settings_window_load(Window* parentWindow) {
  
  // Now we prepare to initialize the menu layer
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  s_settings_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_settings_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = settings_get_num_sections_callback,
    .get_num_rows = settings_get_num_rows_callback,
    .get_header_height = settings_get_header_height_callback,
    .draw_header = settings_draw_header_callback,
    .draw_row = settings_draw_row_callback,
    .select_click = settings_select_callback,
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_settings_layer, parentWindow);
  layer_add_child(window_layer, menu_layer_get_layer(s_settings_layer));

  // Setup sub-windows that we might want to jump to 
  createSettingsSubWin(&s_chevo_window, &s_chevo_context);
  createSettingsSubWin(&s_unique_window, &s_unique_context);

}

void settings_window_unload() {
  menu_layer_destroy(s_settings_layer);
  // Destroy sub-windows
  window_destroy(s_chevo_window);
  window_destroy(s_unique_window);
}