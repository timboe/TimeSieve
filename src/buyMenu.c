#include <pebble.h>
#include "buyMenu.h"
#include "constants.h"
#include "persistence.h"
#include "resources.h"
#include "timeStore.h"

// Menu layers for my windows
static MenuLayer* s_menu_layer = NULL;
static MenuLayer* s_refinery_layer = NULL;
static MenuLayer* s_tank_layer = NULL;
static MenuLayer* s_watcher_layer = NULL;

// My sub-windows
static Window* s_refinery_window;
static Window* s_tank_window;
static Window* s_watcher_window;

static int s_refinery_context = REFINERY_ID;
static int s_tank_context = TANK_ID;
static int s_watcher_context = WATCHER_ID;

static bool s_selectedMaxLevel;

// Temp buffer
static char tempBuffer[TEXT_BUFFER_SIZE];

void updateBuyLayer() {
  if (s_menu_layer != NULL) layer_mark_dirty(menu_layer_get_layer(s_menu_layer));
  if (s_refinery_layer != NULL) layer_mark_dirty(menu_layer_get_layer(s_refinery_layer));
  if (s_tank_layer != NULL) layer_mark_dirty(menu_layer_get_layer(s_tank_layer));
  if (s_watcher_layer != NULL) layer_mark_dirty(menu_layer_get_layer(s_watcher_layer));
}

///
/// MAIN BUY WINDOW CALLBACKS
///

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return NUM_BUY_MENU_SECTIONS; }

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return NUM_BUY_MENU_ITEMS; }

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return MENU_CELL_BASIC_HEADER_HEIGHT; }

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {menu_cell_basic_header_draw(ctx, cell_layer, "BUY Upgrades");}

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
    menu_cell_basic_draw(ctx, cell_layer, "REFINERY", "Get more liquid time", NULL);
  } else if (row == TANK_ID) {
    menu_cell_basic_draw(ctx, cell_layer, "TANK", "Store more liquid time", NULL);
  } else if (row == WATCHER_ID) {
    menu_cell_basic_draw(ctx, cell_layer, "EMPLOYEES", "They work for YOU", NULL);
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
//  } else if (row == SIEVE_ID) {
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH SIEVE");
//    window_stack_push(s_sieve_window, true);
  } else if (row == WATCHER_ID) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH WATCHER");
    window_stack_push(s_watcher_window, true);
  }

}

///
/// REFINARY BUY WINDOW CALLBACKS
///

static uint16_t sub_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return 1; }

static uint16_t sub_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  const int context = *((int*)data);
  if (context == REFINERY_ID) return N_REFINERY_UPGRADES;
  else if (context == TANK_ID) return N_TANK_UPGRADES;
  else if (context == WATCHER_ID) return N_WATCHER_UPGRADES;
  return 0;
}

static int16_t sub_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return MENU_CELL_LARGE_HEADER_HEIGHT; }

static int16_t sub_menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) { return MENU_THREE_CELL_HEIGHT; }

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
  bool maxLevel = false;

  char upgradeText[TEXT_BUFFER_SIZE];
  if (context == REFINERY_ID) {
    upgradeName = NAME_REFINERY[location];
    reward = REWARD_REFINERY[location];
    strcpy(upgradeText, "GAIN: ");
  } else if (context == TANK_ID) {
    upgradeName = NAME_TANK[location];
    reward = REWARD_TANK[location];
    strcpy(upgradeText, "SIZE: ");
  } else if (context == WATCHER_ID) {
    upgradeName = NAME_WATCHER[location];
    reward = REWARD_WATCHER[location];
    bool doneNotifyTxt = false;
    uint8_t owned = getUserOwnsUpgrades(WATCHER_ID, row);
    switch (row) {
      case WATCHER_CHANCE_1: strcpy(upgradeText, "AUTO-Collect +1%"); break;
      case WATCHER_CHANCE_2: strcpy(upgradeText, "AUTO-Collect +5%"); break;
      case WATCHER_FREQUENCY_1: strcpy(upgradeText, "ITEM Chance +0.5%"); break;
      case WATCHER_FREQUENCY_2: strcpy(upgradeText, "ITEM Chance +3%"); break;
      case WATCHER_QUALITY_1: strcpy(upgradeText,   "ITEM Quality 1%"); break;
      case WATCHER_QUALITY_2: strcpy(upgradeText,   "ITEM Qaulity 2.5%"); break;
      case WATCHER_TECH:
        strcpy(upgradeText, "ADDON: ");
        if (owned == TECH_NONE) strcat(upgradeText, "Battery");
        else if (owned == TECH_BATTERY) strcat(upgradeText, "Month");
        else if (owned == TECH_MONTH) strcat(upgradeText, "Weather");
        else maxLevel = true;
        break;
      case WATCHER_LIGHT:
        strcpy(upgradeText, "LIGHT: ");
        doneNotifyTxt = true;
        // DELIBERATE FALL-TRHOUGH
      case WATCHER_VIBE:
        if (doneNotifyTxt == false) {
          strcpy(upgradeText, "VIBE: ");
        }
        if (owned == NOTIFY_NONE) strcat(upgradeText, "Common+");
        else if (owned == NOTIFY_COMMON) strcat(upgradeText, "Magic+");
        else if (owned == NOTIFY_MAGIC) strcat(upgradeText, "Epic+");
        else if (owned == NOTIFY_RARE) strcat(upgradeText, "Rare+");
        else if (owned == NOTIFY_EPIC) strcat(upgradeText, "Legendary");
        else maxLevel = true;
        break;
      case WATCHER_FONT:
        if (owned < FONT_MAX-1) snprintf(upgradeText, TEXT_BUFFER_SIZE, "FONT: %i", (int)(owned+1));
        else maxLevel = true;
        break;
      case WATCHER_COLOUR:
        strcpy(upgradeText, "THEME: ");
        if (owned == PALETTE_BLUE) strcat(upgradeText, "Green");
        else if (owned == PALETTE_GREEN) strcat(upgradeText, "Yellow");
        else if (owned == PALETTE_YELLOW) strcat(upgradeText, "Red");
        else maxLevel = true;
        break;
    }
    if (maxLevel) strcpy(upgradeText, "MAX");

  }
  if (selected) s_selectedMaxLevel = maxLevel;

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
    // Change the name to ?s
    strcpy(tempBuffer, "");
    for (uint8_t i = 0; upgradeName[i] != 0; i++) {
      if (upgradeName[i] == ' ') strcat(tempBuffer, " ");
      else strcat(tempBuffer, "?");
    }
    graphics_draw_text(ctx, tempBuffer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  } else {
    // Now text
    GRect ttlTextRect = GRect(MENU_X_OFFSET, -6,  size.w-MENU_X_OFFSET, size.h);
    GRect topTextRect = GRect(MENU_X_OFFSET, 16, size.w-MENU_X_OFFSET, size.h-22);
    GRect medTextRect = GRect(MENU_X_OFFSET, 27, size.w-MENU_X_OFFSET, size.h-33);
    GRect botTextRect = GRect(MENU_X_OFFSET, 38, size.w-MENU_X_OFFSET, size.h-44);

    timeToString(priceNext, tempBuffer, TEXT_BUFFER_SIZE, true);
    static char s_header[TEXT_BUFFER_SIZE];
    strcpy(s_header, "COST: ");
    strcat(s_header, tempBuffer);
    if (maxLevel) strcpy(s_header, "COST: -");

    static char s_owned[TEXT_BUFFER_SIZE];
    snprintf(s_owned, TEXT_BUFFER_SIZE, "OWNED: %i", owned);

    static char s_reward[TEXT_BUFFER_SIZE];
    strcpy(s_reward, upgradeText);
    if (context != WATCHER_ID) {
      timeToString(reward, tempBuffer, TEXT_BUFFER_SIZE, true);
      strcat(s_reward, tempBuffer);
    }

    graphics_draw_text(ctx, upgradeName, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_header, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_owned, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), medTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_reward, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  }

  graphics_context_set_stroke_color(ctx, GColorBlack);

  // Image placeholder
  GRect imageRect = GRect(3, 10,  22, 36);
  graphics_draw_rect(ctx, imageRect);

  graphics_draw_line(ctx, GPoint(0,0), GPoint(size.w, 0) );

}

/**
 * User is trying to buy something, trigger the purchase, will only go through if it is affordable
 */
static void sub_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  if ( s_selectedMaxLevel == true ) return; // Cannot buy as have maximum
  const int context = *((int*)data);
  const bool result = doPurchase(context, cell_index->row);
  if (result) updateDisplayTime( getUserTime() );
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

/**
 * User is trying to buy as many thing as possible. TODO imp this by making a getIsMaxLevel fn
 */
static void sub_menu_long_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  if ( s_selectedMaxLevel == true ) return; // Cannot buy as have maximum
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

  MenuLayer* new_layer = menu_layer_create(bounds);
  const int context = *((int*) window_get_user_data(parentWindow));
  APP_LOG(APP_LOG_LEVEL_DEBUG,"BUY SUB-WIN %i CREATE", context);

  if (context == REFINERY_ID) {
    s_refinery_layer = new_layer;
    initRefineryWindowRes();
  } else if (context == TANK_ID) {
    s_tank_layer = new_layer;
    initTankWindowRes();
  } else if (context == WATCHER_ID) {
    s_watcher_layer = new_layer;
    initEmployeeWindowRes();
  }
  menu_layer_set_callbacks(new_layer, window_get_user_data(parentWindow), (MenuLayerCallbacks){
    .get_num_sections = sub_menu_get_num_sections_callback,
    .get_num_rows = sub_menu_get_num_rows_callback,
    .get_cell_height = sub_menu_get_cell_height_callback,
    .get_header_height = sub_menu_get_header_height_callback,
    .draw_header = sub_menu_draw_header_callback,
    .draw_row = sub_menu_draw_row_callback,
    .select_click = sub_menu_select_callback,
    .select_long_click = sub_menu_long_callback,
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(new_layer, parentWindow);
  layer_add_child(window_layer, menu_layer_get_layer(new_layer));
}

void sub_window_unload(Window* parentWindow) {
  const int context = *((int*) window_get_user_data(parentWindow));
  APP_LOG(APP_LOG_LEVEL_DEBUG,"BUY SUB-WIN %i DESTROY", context);
  if (context == REFINERY_ID) {
    menu_layer_destroy(s_refinery_layer);
    s_refinery_layer = NULL;
    deinitRefineryWindowRes();
  } else if (context == TANK_ID) {
    menu_layer_destroy(s_tank_layer);
    s_tank_layer = NULL;
    deinitTankWindowRes();
  } else if (context == WATCHER_ID) {
    menu_layer_destroy(s_watcher_layer);
    s_watcher_layer = NULL;
    deinitEmployeeWindowRes();
  }
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
  createSubWin(&s_tank_window, &s_tank_context);
  createSubWin(&s_watcher_window, &s_watcher_context);
  APP_LOG(APP_LOG_LEVEL_DEBUG,"BUY WIN LOAD");

}

void buy_window_unload() {
  APP_LOG(APP_LOG_LEVEL_DEBUG,"BUY WIN DESTROY");
  menu_layer_destroy(s_menu_layer);
  s_menu_layer = NULL;

  // Destroy sub-windows
  window_destroy(s_refinery_window);
  window_destroy(s_tank_window);
  window_destroy(s_watcher_window);
  s_refinery_window = NULL;
  s_tank_window = NULL;
  s_watcher_window = NULL;
}
