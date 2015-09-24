#include <pebble.h>
#include "buyMenuSub.h"
#include "notify.h"
#include "timeStore.h"
#include "resources.h"
#include "persistence.h"
#include "constants.h"

static MenuLayer* s_subMenuLayer = NULL;

static char s_notifyTop[TEXT_BUFFER_SIZE];
static char s_notifyBottom[TEXT_BUFFER_SIZE];

static char s_title[TEXT_BUFFER_SIZE];
static char s_subTitle[TEXT_BUFFER_SIZE];

static char s_header[TEXT_BUFFER_SIZE];
static char s_owned[TEXT_BUFFER_SIZE];
static char s_reward[TEXT_BUFFER_SIZE];

///
/// BUY SUB WINDOW CALLBACKS
///

static uint16_t sub_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return 1; }

static uint16_t sub_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  const int context = *((int*)data);
  if (context == REFINERY_ID) return N_REFINERY_UPGRADES;
  else if (context == TANK_ID) return N_TANK_UPGRADES;
  else if (context == WATCHER_ID) return N_WATCHER_UPGRADES;
  return 1;
}

static int16_t sub_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return MENU_CELL_LARGE_HEADER_HEIGHT; }

static int16_t sub_menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) { return MENU_THREE_CELL_HEIGHT; }

static void sub_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  const int context = *((int*)data);
  const GSize size = layer_get_frame(cell_layer).size;
  timeToString(getUserTime(), getTempBuffer(), TEXT_BUFFER_SIZE, true);


  if (context == REFINERY_ID) strcpy(s_title, "REFINARY Upgrades");
  else if (context == TANK_ID) strcpy(s_title, "TANK Upgrades");
  else if (context == WATCHER_ID) strcpy(s_title, "WATCHER Upgrades");
  GRect topTextRect = GRect(2, 0, size.w-2, size.h);
  GRect botTextRect = GRect(2, 14, size.w-2, size.h-14);
  strcpy(s_subTitle, "AVAILABLE: ");
  strcat(s_subTitle, getTempBuffer());

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_title, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, s_subTitle, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void sub_menu_draw_row_callback(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* data) {

  const int context = *((int*)data);
  const GSize size = layer_get_frame(cell_layer).size;
  const int row = cell_index->row;
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);

  //Get details
  const unsigned location = cell_index->row;
  const int owned = getUserUpgrades(context, location);
  const uint64_t priceNext = getPriceOfUpgrade(context, location);
  bool maxLevel = getIfAtMaxLevel(context, location);
  GBitmap* buyImage = getBuyImage(context, location);

  char upgradeText[TEXT_BUFFER_SIZE];
  if (context == REFINERY_ID) {
    strcpy(upgradeText, "GAIN: ");
  } else if (context == TANK_ID) {
    strcpy(upgradeText, "SIZE: ");
  } else if (context == WATCHER_ID) {
    bool doneNotifyTxt = false;
    uint8_t owned = getUserUpgrades(WATCHER_ID, row);
    switch (row) {
      case WATCHER_CHANCE_1: strcpy(upgradeText, "AUTO-Collect +1%"); break;
      case WATCHER_CHANCE_2: strcpy(upgradeText, "AUTO-Collect +5%"); break;
      case WATCHER_FREQUENCY_1: strcpy(upgradeText, "ITEM Chance +0.5%"); break;
      case WATCHER_FREQUENCY_2: strcpy(upgradeText, "ITEM Chance +3%"); break;
      case WATCHER_QUALITY_1: strcpy(upgradeText,   "ITEM Quality +1%"); break;
      case WATCHER_QUALITY_2: strcpy(upgradeText,   "ITEM Qaulity +4%"); break;
      case WATCHER_TECH:
        strcpy(upgradeText, "ADDON: ");
        if (owned == TECH_NONE) strcat(upgradeText, "Battery");
        else if (owned == TECH_BATTERY) strcat(upgradeText, "Month");
        else if (owned == TECH_MONTH) strcat(upgradeText, "Weather");
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
        break;
      case WATCHER_FONT:
        if (owned < FONT_MAX-1) snprintf(upgradeText, TEXT_BUFFER_SIZE, "FONT: %i", (int)(owned+1));
        break;
      case WATCHER_COLOUR:
        strcpy(upgradeText, "THEME: ");
        if (owned == PALETTE_BLUE) strcat(upgradeText, "Green");
        else if (owned == PALETTE_GREEN) strcat(upgradeText, "Yellow");
        else if (owned == PALETTE_YELLOW) strcat(upgradeText, "Red");
        break;
    }
    if (maxLevel) strcpy(upgradeText, "MAX");
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
    // Change the name to ?s
    strcpy(getTempBuffer(), "");
    for (uint8_t i = 0; (UPGRADE_NAME[context][location])[i] != 0; i++) {
      if ((UPGRADE_NAME[context][location])[i] == ' ') strcat(getTempBuffer(), " ");
      else strcat(getTempBuffer(), "?");
    }
    graphics_draw_text(ctx, getTempBuffer(), fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    buyImage = getQImage();
  } else {
    // Now text
    GRect ttlTextRect = GRect(MENU_X_OFFSET, -6,  size.w-MENU_X_OFFSET, size.h);
    GRect topTextRect = GRect(MENU_X_OFFSET, 16, size.w-MENU_X_OFFSET, size.h-22);
    GRect medTextRect = GRect(MENU_X_OFFSET, 27, size.w-MENU_X_OFFSET, size.h-33);
    GRect botTextRect = GRect(MENU_X_OFFSET, 38, size.w-MENU_X_OFFSET, size.h-44);

    timeToString(priceNext, getTempBuffer(), TEXT_BUFFER_SIZE, true);
    strcpy(s_header, "COST: ");
    strcat(s_header, getTempBuffer());
    if (maxLevel) strcpy(s_header, "COST: -");

    snprintf(s_owned, TEXT_BUFFER_SIZE, "OWNED: %i", owned);

    strcpy(s_reward, upgradeText);
    if (context != WATCHER_ID) {
      timeToString(UPGRADE_REWARD[context][location], getTempBuffer(), TEXT_BUFFER_SIZE, true);
      strcat(s_reward, getTempBuffer());
    }

    graphics_draw_text(ctx, UPGRADE_NAME[context][location], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_header, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_owned, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), medTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_reward, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  }

  graphics_context_set_stroke_color(ctx, GColorBlack);

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, buyImage, GRect(3, 10,  22, 36));

}

static void buyClick(const uint32_t typeID, const uint32_t resourceID, const bool buyTen) {
  uint8_t bought = 0;
  uint64_t paid = 0;
  for (uint8_t i = 0; i < (buyTen ? 10 : 1); ++i) {
    const uint64_t price = doPurchase(typeID, resourceID);
    paid += price;
    if (price > 0) ++bought;
    else break;
  }
  GColor highlight = GColorBlack;
  if (bought) updateDisplayTime( getUserTime() );
  snprintf(s_notifyTop, TEXT_BUFFER_SIZE, "Bought %i", (int)bought);
  if (bought == 0) {
    if (getIfAtMaxLevel(typeID, resourceID)) strcpy(s_notifyBottom, "Own Maximum!");
    else strcpy(s_notifyBottom, "Insufficient Time!");
    highlight = GColorRed;
  } else {
    timeToString(paid, getTempBuffer(), TEXT_BUFFER_SIZE, true);
    strcpy(s_notifyBottom, "For ");
    strcat(s_notifyBottom, getTempBuffer());
  }

  showNotify(highlight, s_notifyTop, UPGRADE_NAME[typeID][resourceID], s_notifyBottom);
}

/**
 * User is trying to buy something, trigger the purchase, will only go through if it is affordable
 */
static void sub_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  const int context = *((int*)data);
  buyClick(context, cell_index->row, false);
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

/**
 * User is trying to buy 10. TODO imp this by making a getIsMaxLevel fn
 */
static void sub_menu_long_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  const int context = *((int*)data);
  buyClick(context, cell_index->row, true);
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}




///
/// SETUP
///

static void sub_window_load(Window* parentWindow) {
  // Now we prepare to initialize the menu layer
  s_subMenuLayer = menu_layer_create(layer_get_frame(window_get_root_layer(parentWindow)));
  const int context = *((int*) window_get_user_data(parentWindow));

  menu_layer_set_callbacks(s_subMenuLayer, window_get_user_data(parentWindow), (MenuLayerCallbacks){
    .get_num_sections = sub_menu_get_num_sections_callback,
    .get_num_rows = sub_menu_get_num_rows_callback,
    .get_cell_height = sub_menu_get_cell_height_callback,
    .get_header_height = sub_menu_get_header_height_callback,
    .draw_header = sub_menu_draw_header_callback,
    .draw_row = sub_menu_draw_row_callback,
    .select_click = sub_menu_select_callback,
    .select_long_click = sub_menu_long_callback
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_normal_colors(s_subMenuLayer, MENU_BACK_GREEN_ODD, GColorBlack);
  menu_layer_set_click_config_onto_window(s_subMenuLayer, parentWindow);
  layer_add_child(window_get_root_layer(parentWindow), menu_layer_get_layer(s_subMenuLayer));

  layer_add_child(window_get_root_layer(parentWindow), getNotifyLayer());
  initBuyWindowRes(context);
}

static void sub_window_unload(Window* parentWindow) {
  destroyNotifyLayer();
  const int context = *((int*) window_get_user_data(parentWindow));
  deinitBuyWindowRes(context);
  APP_LOG(APP_LOG_LEVEL_DEBUG,"SWUL");
  menu_layer_destroy(s_subMenuLayer);
  s_subMenuLayer = NULL;
}


void createSubWin(Window** w, int* context) {
  *w = window_create();
  window_set_user_data(*w, context);
  window_set_window_handlers(*w, (WindowHandlers) {
    .load = sub_window_load,
    .unload = sub_window_unload
  });
}

void updateBuyMenuSubLayer() {
  if (s_subMenuLayer != NULL) layer_mark_dirty(menu_layer_get_layer(s_subMenuLayer));
}
