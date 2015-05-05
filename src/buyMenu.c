#include <pebble.h>
#include "buyMenu.h"
#include "constants.h"
#include "persistence.h"
#include "timeStore.h"
#include "ditheredRect.h"
  

  
// Menu layers for my windows
static MenuLayer* s_menu_layer;  
static MenuLayer* s_refinery_layer;  
static MenuLayer* s_tank_layer;
static MenuLayer* s_sieve_layer;
static MenuLayer* s_watcher_layer;

// My sub-windows
static Window* s_refinery_window;
static Window* s_tank_window;
static Window* s_sieve_window;
static Window* s_watcher_window;

static int s_refinery_context = REFINERY_ID;
static int s_sieve_context = SIEVE_ID;
static int s_tank_context = TANK_ID;
static int s_watcher_context = WATCHER_ID;

// Temp buffer
static char tempBuffer[TEXT_BUFFER_SIZE];

/// 
/// MAIN BUY WINDOW CALLBACKS
///

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return NUM_BUY_MENU_SECTIONS; }

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return NUM_BUY_MENU_ITEMS; }

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return 0; }

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  const GSize size = layer_get_frame(cell_layer).size;
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);
  const int row = cell_index->row;
  if (selected)        graphics_context_set_fill_color(ctx, MENU_BACK_GREEN_SELECT);
  else if (row%2 == 1) graphics_context_set_fill_color(ctx, MENU_BACK_GREEN_ODD);
  else                 graphics_context_set_fill_color(ctx, MENU_BACK_GREEN_EVEN);
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);
  if (row == REFINERY_ID) { 
    menu_cell_basic_draw(ctx, cell_layer, "REFINERY Upgrades", "Get more liquid time", NULL);
  } else if (row == TANK_ID) {
    menu_cell_basic_draw(ctx, cell_layer, "TANK Upgrades", "Store more liquid time", NULL);
  } else if (row == SIEVE_ID) {
    menu_cell_basic_draw(ctx, cell_layer, "SIEVE Upgrades", "Find more treasures", NULL);
  } else if (row == WATCHER_ID) {
    menu_cell_basic_draw(ctx, cell_layer, "WATCHERS", "Collect more treasure", NULL);
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  const int row = cell_index->row;
  if (row == REFINERY_ID) { 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH REFINERY");
    window_stack_push(s_refinery_window, true);
  } else if (row == TANK_ID) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH TANK");
    window_stack_push(s_tank_window, true);
  } else if (row == SIEVE_ID) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH SIEVE");
    window_stack_push(s_sieve_window, true);
  } else if (row == WATCHER_ID) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH WATCHER");
    window_stack_push(s_watcher_window, true);
  }
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

static uint16_t sub_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return 1; }

static uint16_t sub_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { 
  const int context = *((int*)data);
  if (context == REFINERY_ID) return N_REFINERY_UPGRADES;
  else if (context == TANK_ID) return N_TANK_UPGRADES;
  else if (context == SIEVE_ID) return N_SIEVE_UPGRADES;
  else if (context == WATCHER_ID) return N_WATCHER_UPGRADES;
  return 0;
}

static int16_t sub_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return MENU_CELL_LARGE_HEADER_HEIGHT; }

static int16_t sub_menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) { return MENU_CELL_HEIGHT; }

static void sub_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "HEADER DRAW %i", section_index);

  const int context = *((int*)data);
  const GSize size = layer_get_frame(cell_layer).size;
  timeToString(getUserTime(), tempBuffer, TEXT_BUFFER_SIZE, true);
  static char s_title[TEXT_BUFFER_SIZE];
  static char s_header[TEXT_BUFFER_SIZE];
  strcpy(s_header, "AVAILABLE: ");
  strcat(s_header, tempBuffer);
  if (context == REFINERY_ID) strcpy(s_title, "REFINARY Upgrades");
  else if (context == TANK_ID) strcpy(s_title, "TANK Upgrades");
  else if (context == SIEVE_ID) strcpy(s_title, "SIEVE Upgrades");
  else if (context == WATCHER_ID) strcpy(s_title, "WATCHER Upgrades");
  GRect topTextRect = GRect(2, 0, size.w-2, size.h);
  GRect botTextRect = GRect(2, 14, size.w-2, size.h-14);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_title, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, s_header, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void sub_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "ROW DRAW %i", cell_index->row);

  const int context = *((int*)data);
  const GSize size = layer_get_frame(cell_layer).size;
  const int row = cell_index->row;
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);
  
  //Get details
  const unsigned location = cell_index->row;
  const int owned = getUserOwnsUpgrades(context, location);
  const uint64_t priceNext = getPriceOfUpgrade(context, location);
  const char* upgradeName = NULL;
  uint64_t reward = 0;

  char upgradeText[TEXT_BUFFER_SIZE];
  if (context == REFINERY_ID) {
    upgradeName = NAME_REFINERY[location];
    reward = REWARD_REFINERY[location];
    strcpy(upgradeText, "GAIN: ");
  } else if (context == TANK_ID) {
    upgradeName = NAME_TANK[location];
    reward = REWARD_TANK[location];
    strcpy(upgradeText, "SIZE: ");
  } else if (context == SIEVE_ID) {
    upgradeName = NAME_SIEVE[location];
    reward = REWARD_SIEVE[location];
    strcpy(upgradeText, "CHANCE: ");
  } else if (context == WATCHER_ID) {
    upgradeName = NAME_WATCHER[location];
    reward = REWARD_WATCHER[location];
    strcpy(upgradeText, "CHANCE: ");
  }

  bool display = false, canAfford = false;
  // Can I afford this? Only display if total time gained playing is greater than the cost
  if (owned > 0 || getUserTotalTime() > priceNext) display = true;
  if (getUserTime() >= priceNext) canAfford = true;

  // Text colours
  if (selected) graphics_context_set_text_color(ctx, GColorWhite);
  else graphics_context_set_text_color(ctx, GColorBlack);

  // Back color
  GColor backColor;
  if (selected && !display)         backColor = GColorDarkGray;
  else if (!display)                backColor = GColorLightGray;
  else if (selected && canAfford)   backColor = MENU_BACK_GREEN_SELECT;
  else if (selected)                backColor = MENU_BACK_RED_SELECT;
  else if (row%2 == 1 && canAfford) backColor = MENU_BACK_GREEN_ODD;
  else if (canAfford)               backColor = MENU_BACK_GREEN_EVEN;
  else if (row%2 == 1)              backColor = MENU_BACK_RED_ODD;
  else                              backColor = MENU_BACK_RED_EVEN;
  graphics_context_set_fill_color(ctx, backColor);
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);

  // Do not have and cannot afford, draw ???s
  if (display == false) {
    //draw_dithered_rect(ctx, GRect(0, 0, size.w, size.h), GColorLightGray, GColorWhite, DITHER_10_PERCENT);
    GRect ttlTextRect = GRect(MENU_X_OFFSET, 0,  size.w-MENU_X_OFFSET, size.h);
    graphics_draw_text(ctx, "??????", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  } else {
    // Now text
    GRect ttlTextRect = GRect(MENU_X_OFFSET, 0,  size.w-MENU_X_OFFSET, size.h);
    GRect topTextRect = GRect(MENU_X_OFFSET, 22, size.w-MENU_X_OFFSET, size.h-22);
    GRect medTextRect = GRect(MENU_X_OFFSET, 33, size.w-MENU_X_OFFSET, size.h-33);
    GRect botTextRect = GRect(MENU_X_OFFSET, 44, size.w-MENU_X_OFFSET, size.h-44);
    
    timeToString(priceNext, tempBuffer, TEXT_BUFFER_SIZE, true);
    static char s_header[TEXT_BUFFER_SIZE];
    strcpy(s_header, "COST: ");
    strcat(s_header, tempBuffer);

    static char s_owned[TEXT_BUFFER_SIZE];
    snprintf(s_owned, TEXT_BUFFER_SIZE, "OWNED: %i", owned);

    timeToString(reward, tempBuffer, TEXT_BUFFER_SIZE, true);
    static char s_reward[TEXT_BUFFER_SIZE];
    strcpy(s_reward, upgradeText);
    strcat(s_reward, tempBuffer);

    graphics_draw_text(ctx, upgradeName, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_header, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_owned, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), medTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_reward, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  }

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, GPoint(0,0), GPoint(size.w, 0) );

}

/**
 * User is trying to buy something, trigger the purchase, will only go through if it is affordable
 */
static void sub_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  const int context = *((int*)data);
  const bool result = doPurchase(context, cell_index->row);  
  if (result) updateDisplayTime( getUserTime() );
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

/// 
/// SETUP
///

void sub_window_load(Window* parentWindow) {
  // Now we prepare to initialize the menu layer
  Layer* window_layer = window_get_root_layer(parentWindow);
  const GRect bounds = layer_get_frame(window_layer);

  MenuLayer* new_layer;
  const int context = *((int*) window_get_user_data(parentWindow));
  APP_LOG(APP_LOG_LEVEL_DEBUG,"BUY SUB-WIN %i CREATE", context);

  if (context == REFINERY_ID)     new_layer = s_refinery_layer;
  else if (context == SIEVE_ID)   new_layer = s_sieve_layer;
  else if (context == TANK_ID)    new_layer = s_tank_layer;
  else if (context == WATCHER_ID) new_layer = s_watcher_layer;
  // Create the menu layer
  new_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(new_layer, window_get_user_data(parentWindow), (MenuLayerCallbacks){
    .get_num_sections = sub_menu_get_num_sections_callback,
    .get_num_rows = sub_menu_get_num_rows_callback,
    .get_cell_height = sub_menu_get_cell_height_callback,
    .get_header_height = sub_menu_get_header_height_callback,
    .draw_header = sub_menu_draw_header_callback,
    .draw_row = sub_menu_draw_row_callback,
    .select_click = sub_menu_select_callback,
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(new_layer, parentWindow);
  layer_add_child(window_layer, menu_layer_get_layer(new_layer));
}

void sub_window_unload(Window* parentWindow) {
  const int context = *((int*) window_get_user_data(parentWindow));
  APP_LOG(APP_LOG_LEVEL_DEBUG,"BUY SUB-WIN %i DESTROY", context);
  if (context == REFINERY_ID)     menu_layer_destroy(s_refinery_layer);
  else if (context == SIEVE_ID)   menu_layer_destroy(s_sieve_layer);
  else if (context == TANK_ID)    menu_layer_destroy(s_tank_layer);
  else if (context == WATCHER_ID) menu_layer_destroy(s_watcher_layer);
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
void createSubWin(Window** w, int* context) {
  *w = window_create();
  window_set_user_data(*w, context);
  window_set_background_color(*w, MENU_BACK_GREEN_ODD);
  window_set_window_handlers(*w, (WindowHandlers) {
    .load = sub_window_load,
    .unload = sub_window_unload
  }); 
}

void buy_window_load(Window* parentWindow) {
  
  // Now we prepare to initialize the menu layer
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, parentWindow);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

  // Setup sub-windows that we might want to jump to 
  createSubWin(&s_refinery_window, &s_refinery_context);
  createSubWin(&s_sieve_window, &s_sieve_context);
  createSubWin(&s_tank_window, &s_tank_context);
  createSubWin(&s_watcher_window, &s_watcher_context);
  APP_LOG(APP_LOG_LEVEL_DEBUG,"BUY WIN LOAD");

}

void buy_window_unload() {
  APP_LOG(APP_LOG_LEVEL_DEBUG,"BUY WIN DESTROY");
  menu_layer_destroy(s_menu_layer);

  // Destroy sub-windows
  window_destroy(s_refinery_window);
  window_destroy(s_tank_window);
  window_destroy(s_sieve_window);
  window_destroy(s_watcher_window);
}