#include <pebble.h>
#include "settingsMenu.h"
#include "constants.h"
#include "persistence.h"
#include "timeStore.h"
#include "sellMenu.h"
#include "resources.h"
#include "items.h"

#define CHEVO_CONTEXT_ID 0
#define UNIQUE_CONTEXT_ID 1

#define STAT_SECTION_ID 0
#define CHEVO_SECTION_ID 1
#define UNLOCK_SECTION_ID 2
#define SETTINGS_SECTION_ID 3

#define NUM_SETTINGS_MENU_SECTIONS 3
//
#define NUM_STAT_ROWS 3
#define NUM_CHEVO_ROWS 2
#define NUM_UNLOCK_ROWS 5

static MenuLayer* s_settings_layer = NULL;
static MenuLayer* s_settingsSubLayer = NULL;

static Window* s_chevo_window = NULL;
static Window* s_unique_window = NULL;

static int s_chevo_context = CHEVO_CONTEXT_ID;
static int s_unique_context = UNIQUE_CONTEXT_ID;

static char tempBuffer[TEXT_BUFFER_SIZE];

static uint8_t s_timeDisplay = 0;
static uint8_t s_buildingDisplay = 0;
static int8_t s_itemDisplay = -1;

// Hold unlockable settings and which are currently locked
#define UNLOCK_TECH_ID 0
#define UNLOCK_LIGHT_ID 1
#define UNLOCK_FONT_ID 2
#define UNLOCK_VIBE_ID 3
#define UNLOCK_COLOUR_ID 4

static uint8_t s_unlockSetting[NUM_UNLOCK_ROWS];
static uint8_t s_unlockedTo[NUM_UNLOCK_ROWS]; // How far has the user actually unlocked?

void updateSettingsLayer() {
  if (s_settings_layer != NULL) layer_mark_dirty(menu_layer_get_layer(s_settings_layer));
  if (s_settingsSubLayer != NULL) layer_mark_dirty(menu_layer_get_layer(s_settingsSubLayer));
}

void loadUserSettings() {
  s_unlockSetting[UNLOCK_TECH_ID]   = getUserSetting(SETTING_TECH);
  s_unlockSetting[UNLOCK_LIGHT_ID]  = getUserSetting(SETTING_LIGHT);
  s_unlockSetting[UNLOCK_FONT_ID]   = getUserSetting(SETTING_TYPE);
  s_unlockSetting[UNLOCK_VIBE_ID]   = getUserSetting(SETTING_VIBE);
  s_unlockSetting[UNLOCK_COLOUR_ID] = getUserSetting(SETTING_COLOUR);

  s_unlockedTo[UNLOCK_TECH_ID]   = getUserUpgrades(WATCHER_ID, WATCHER_TECH);
  s_unlockedTo[UNLOCK_LIGHT_ID]  = getUserUpgrades(WATCHER_ID, WATCHER_LIGHT);
  s_unlockedTo[UNLOCK_FONT_ID]   = getUserUpgrades(WATCHER_ID, WATCHER_FONT);
  s_unlockedTo[UNLOCK_VIBE_ID]   = getUserUpgrades(WATCHER_ID, WATCHER_VIBE);
  s_unlockedTo[UNLOCK_COLOUR_ID] = getUserUpgrades(WATCHER_ID, WATCHER_COLOUR);
}

///
/// SETTINGS WINDOW CALLBACKS
///

static uint16_t settings_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return NUM_SETTINGS_MENU_SECTIONS; }

static uint16_t settings_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
  	case STAT_SECTION_ID: return NUM_STAT_ROWS;
  	case CHEVO_SECTION_ID: return NUM_CHEVO_ROWS;
  	case UNLOCK_SECTION_ID: return NUM_UNLOCK_ROWS;
  	default: return 0;
  }
}

static int16_t settings_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return MENU_CELL_BASIC_HEADER_HEIGHT; }

static void settings_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  const GSize size = layer_get_frame(cell_layer).size;
  if (section_index == STAT_SECTION_ID) {
    strcpy(tempBuffer, "STATISTICS");
    graphics_context_set_fill_color(ctx, MENU_BACK_YELLOW_ODD);
  } else if (section_index == CHEVO_SECTION_ID) {
    strcpy(tempBuffer, "PRESTIGE");
    graphics_context_set_fill_color(ctx, MENU_BACK_GREEN_ODD);
  } else if (section_index == UNLOCK_SECTION_ID) {
    strcpy(tempBuffer, "UNLOCKS");
    graphics_context_set_fill_color(ctx, MENU_BACK_BLUE_ODD);
  }
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);
  menu_cell_basic_header_draw(ctx, cell_layer, tempBuffer);
}

static int16_t settings_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  switch(cell_index->section) {
    case STAT_SECTION_ID: return MENU_TWO_CELL_HEIGHT;
    case CHEVO_SECTION_ID: return MENU_ZERO_CELL_HEIGHT;
    case UNLOCK_SECTION_ID: return MENU_ONE_CELL_HEIGHT;
    default: return 0;
  }
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
  else if (section == UNLOCK_SECTION_ID && selected   ) backColor = MENU_BACK_BLUE_SELECT;
  else if (section == UNLOCK_SECTION_ID && row%2 == 0 ) backColor = MENU_BACK_BLUE_EVEN;
  else if (section == UNLOCK_SECTION_ID               ) backColor = MENU_BACK_BLUE_ODD;

  graphics_context_set_fill_color(ctx, backColor);
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);

  GRect ttlTextRect = GRect(0, -6,  size.w, size.h);
  GRect topTextRect = GRect(0, 16, size.w, size.h-22);
  GRect botTextRect = GRect(0, 27, size.w, size.h-33);

  static char titleText[TEXT_BUFFER_SIZE];
  static char subText1[TEXT_LARGE_BUFFER_SIZE];
  static char subText2[TEXT_BUFFER_SIZE];
  strcpy(titleText, "");
  strcpy(subText1, "");
  strcpy(subText2, "");

  // Stat section
  if (section == STAT_SECTION_ID) {

    if (row == 0) { // DISPLAY TIME

      if (s_timeDisplay == 0) {
        strcpy(titleText, "TOTAL Time");
        timeToString(getUserTime(), subText1, TEXT_LARGE_BUFFER_SIZE, false);
      } else if (s_timeDisplay == 1) {
        strcpy(titleText, "ALLTime");
        timeToString(getUserTotalTime(), subText1, TEXT_LARGE_BUFFER_SIZE, false);
      } else if (s_timeDisplay == 2) {
        strcpy(titleText, "TANK Capacity");
        timeToString(getTankCapacity(), subText1, TEXT_LARGE_BUFFER_SIZE, false);
      } else if (s_timeDisplay == 3) {
        strcpy(titleText, "REFINERY Output");
        timeToString(getTimePerMin(), subText1, TEXT_LARGE_BUFFER_SIZE, false);
      }

    } else if (row == 1) { // DISPLAY BUILDINGS

      int v = 0;
      if (s_buildingDisplay == 0) {
        strcpy(titleText, "ITEM Find");
        v = getFindBaseChance();
      } else if (s_buildingDisplay == 1) {
        strcpy(titleText, "ITEM Quality");
        v = getQualityBaseChance();
      } else if (s_buildingDisplay == 2) {
        strcpy(titleText, "ITEM Auto-Collect");
        v = getAutoCollectChance();
      }
      snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "BONUS: %i.%i%%", v/(SCALE_FACTOR/100), v%(SCALE_FACTOR/100) );

    } else if (row == 2) { // DISPLAY ITEMS

      if      (s_itemDisplay == -1)           strcpy(titleText, "MISSED Items");
      else if (s_itemDisplay == COMMON_ID)    strcpy(titleText, "COMMON Items");
      else if (s_itemDisplay == MAGIC_ID)     strcpy(titleText, "MAGIC Items");
      else if (s_itemDisplay == RARE_ID)      strcpy(titleText, "RARE Items");
      else if (s_itemDisplay == EPIC_ID)      strcpy(titleText, "EPIC Items");
      else if (s_itemDisplay == LEGENDARY_ID) strcpy(titleText, "LEGENDARIES");
      if (s_itemDisplay == -1) {
        snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "TOTAL: %i", (int)getItemsMissed() );
      } else {
        snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "OWNED: %i", (int)getUserTotalItems(s_itemDisplay) );
        strcpy(subText2, "VALUE:");
        timeToString(currentCategorySellPrice(s_itemDisplay), tempBuffer, TEXT_BUFFER_SIZE, true);
        strcat(subText2, tempBuffer);
        if (s_itemDisplay == LEGENDARY_ID) strcpy(subText2, "VALUE: Timeless"); // Cannot sell legendaries
      }
    }

  } else if (section == CHEVO_SECTION_ID) {

    if (row == CHEVO_CONTEXT_ID && getTotalChevos()) {
      strcpy(titleText, "ACHIEVEMENTS");
    } else if (row == UNIQUE_CONTEXT_ID && getUserTotalItems(LEGENDARY_ID)) {
      strcpy(titleText, "LEGENDARIES");
    } else {
      strcpy(titleText, "??????????");
    }

  } else if (section == UNLOCK_SECTION_ID) {

    const uint8_t setting = s_unlockSetting[row];
    const uint8_t lock = s_unlockedTo[row];

    if (lock == 0) { // not unlocked... at all
      strcpy(titleText, "??????");
      strcat(subText1, "?/?] ????");
    } else if (row == UNLOCK_TECH_ID) {

      strcpy(titleText, "TECH Addons");
      snprintf(subText1, TEXT_BUFFER_SIZE, "%i/%i] ", (int)setting+1, TECH_MAX);
      if      (setting > lock) strcat(subText1, "LOCKED");
      else if (setting == TECH_NONE)    strcat(subText1, "NONE");
      else if (setting == TECH_BATTERY) strcat(subText1, "BATTERY");
      else if (setting == TECH_MONTH)   strcat(subText1, "MONTH");
      else if (setting == TECH_WEATHER) strcat(subText1, "WEATHER");

    } else if (row == UNLOCK_LIGHT_ID || row == UNLOCK_VIBE_ID) { // Light or vibe on treasure

      strcpy(titleText, "LIGHT Notify");
      if (row == UNLOCK_VIBE_ID) strcpy(titleText, "VIBRATE Notify");
      snprintf(subText1, TEXT_BUFFER_SIZE, "%i/%i] ", (int)setting+1, NOTIFY_MAX);
      if      (setting > lock) strcat(subText1, "LOCKED");
      else if (setting == NOTIFY_NONE)      strcat(subText1, "OFF");
      else if (setting == NOTIFY_COMMON)    strcat(subText1, "COMMON+");
      else if (setting == NOTIFY_MAGIC)     strcat(subText1, "MAGIC+");
      else if (setting == NOTIFY_RARE)      strcat(subText1, "RARE+");
      else if (setting == NOTIFY_EPIC)      strcat(subText1, "EPIC+");
      else if (setting == NOTIFY_LEGENDARY) strcat(subText1, "LEGENDARY");

    } else if (row == UNLOCK_FONT_ID) { // Typeface

      strcpy(titleText, "CLOCK Typeface");
      snprintf(subText1, TEXT_BUFFER_SIZE, "%i/%i] ", (int)setting+1, FONT_MAX);
      if (setting > lock) snprintf(subText1, TEXT_BUFFER_SIZE, "%i/%i] LOCKED", (int)setting+1, FONT_MAX);
      else                snprintf(subText1, TEXT_BUFFER_SIZE, "%i/%i] TYPEFACE %i", (int)setting+1, FONT_MAX, (int)setting+1);

    } else if (row == UNLOCK_COLOUR_ID) { // Vibe on treasure

      strcpy(titleText, "PALETTE");
      snprintf(subText1, TEXT_BUFFER_SIZE, "%i/%i] ", (int)setting+1, PALETTE_MAX);
      if      (setting > lock) strcat(subText1, "LOCKED");
      else if (setting == PALETTE_BLUE)   strcat(subText1, "BLUE");
      else if (setting == PALETTE_GREEN)  strcat(subText1, "GREEN");
      else if (setting == PALETTE_YELLOW) strcat(subText1, "YELLOW");
      else if (setting == PALETTE_RED)    strcat(subText1, "RED");

    }

  } else {
    return;
  }

  graphics_draw_text(ctx, titleText, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, subText1, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, subText2, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

}

static void settings_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  const uint16_t section = cell_index->section;
  const uint16_t row = cell_index->row;
  // Stat section
  if (section == STAT_SECTION_ID) {

    if (row == 0) {
      if (++s_timeDisplay == 4) s_timeDisplay = 0;
    } else if (row == 1) {
      if (++s_buildingDisplay == 3) s_buildingDisplay = 0;
    } else if (row == 2) {
      // Only show for categories where we have items - no spoils
      while (++s_itemDisplay < 5 && getUserTotalItems(s_itemDisplay) == 0) {}
      if (s_itemDisplay == 5) s_itemDisplay = -1;
    }

  } else if (section == CHEVO_SECTION_ID) {

    if (row == CHEVO_CONTEXT_ID && getTotalChevos()) {
      window_stack_push(s_chevo_window, true);
    } else if (row == UNIQUE_CONTEXT_ID && getUserTotalItems(LEGENDARY_ID)) {
      window_stack_push(s_unique_window, true);
    }

  } else if (section == UNLOCK_SECTION_ID) {

    ++s_unlockSetting[row];
    const uint8_t lock = s_unlockedTo[row];
    if (row == UNLOCK_TECH_ID) {

      if (s_unlockSetting[row] >= TECH_MAX) s_unlockSetting[row] = 0;
      if (s_unlockSetting[row] <= lock)     setUserSetting(SETTING_TECH, s_unlockSetting[row]);

    } else if (row == UNLOCK_LIGHT_ID) {

      if (s_unlockSetting[row] >= NOTIFY_MAX) s_unlockSetting[row] = 0;
      if (s_unlockSetting[row] <= lock)       setUserSetting(SETTING_LIGHT, s_unlockSetting[row]);

    } else if (row == UNLOCK_VIBE_ID) { // Light or vibe on treasure

      if (s_unlockSetting[row] >= NOTIFY_MAX) s_unlockSetting[row] = 0;
      if (s_unlockSetting[row] <= lock)       setUserSetting(SETTING_VIBE, s_unlockSetting[row]);

    } else if (row == UNLOCK_FONT_ID) { // Typeface

      if (s_unlockSetting[row] >= FONT_MAX) s_unlockSetting[row] = 0;
      if (s_unlockSetting[row] <= lock)     setUserSetting(SETTING_TYPE, s_unlockSetting[row]);

    } else if (row == UNLOCK_COLOUR_ID) { // Vibe on treasure

      if (s_unlockSetting[row] >= PALETTE_MAX) s_unlockSetting[row] = 0;
      if (s_unlockSetting[row] <= lock)        setUserSetting(SETTING_COLOUR, s_unlockSetting[row]);

    }

  }
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

///
/// SETTINGS SUB WINDOW CALLBACKS
///

static uint16_t settings_sub_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return 1; }

static uint16_t settings_sub_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  const int context = *((int*)data);
  if (context == CHEVO_CONTEXT_ID) return getTotalChevos();
  else if (context == UNIQUE_CONTEXT_ID) return getUserTotalItems(LEGENDARY_ID);
  return 0;
}

static int16_t settings_sub_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_LARGE_HEADER_HEIGHT;
}

static int16_t settings_sub_menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) { return MENU_TWO_CELL_HEIGHT; }

static void settings_sub_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  const int context = *((int*)data);
  const GSize size = layer_get_frame(cell_layer).size;
  const GRect topTextRect = GRect(2, 0, size.w-2, size.h);
  const GRect botTextRect = GRect(2, 14, size.w-2, size.h-14);
  static char s_subTitle1[TEXT_BUFFER_SIZE];
  static char s_subTitle2[TEXT_BUFFER_SIZE];
  if (context == CHEVO_CONTEXT_ID) {
    strcpy(s_subTitle1, "ACHIEVEMENTS");
    strcpy(s_subTitle2, "1% time bonus each");
  } else if (context == UNIQUE_CONTEXT_ID) {
    strcpy(s_subTitle1, "LEGENDARIES");
    strcpy(s_subTitle2, "With unique bonuses");
  }
  graphics_draw_text(ctx, s_subTitle1, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, s_subTitle2, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void settings_sub_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  const int context = *((int*)data);
  const GSize size = layer_get_frame(cell_layer).size;
  const uint16_t row = cell_index->row;
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);

  int8_t itemID;
  const char* nameText;
  const char* descText;
  GBitmap* image = NULL;
  GColor backColor;
  GRect ttlTextRect = GRect(MENU_X_OFFSET, -6, size.w-MENU_X_OFFSET, size.h);
  GRect topTextRect = GRect(MENU_X_OFFSET, 16, size.w-MENU_X_OFFSET, size.h-22);
  GRect imageRect = GRect(3, 4,  22, 36);
  if (context == UNIQUE_CONTEXT_ID) {
    itemID = getItemIDFromRow(LEGENDARY_ID, row);
    if (itemID == -1) return;
    nameText = NAME_LEGENDARY[itemID];
    descText = DESC_LEGENDARY[itemID];
    image = getPrestigeItemImage(itemID);
    if (row%2==0) {
      ttlTextRect = GRect(3, -6, size.w-MENU_X_OFFSET, size.h);
      topTextRect = GRect(3, 16, size.w-MENU_X_OFFSET, size.h-22);
      imageRect = GRect(size.w-MENU_X_OFFSET+3, 4,  22, 36);
    }
  } else if (context == CHEVO_CONTEXT_ID) { // CHEVO
    itemID = getAchievementIDFromRow(row);
    if (itemID == -1) return;
    nameText = NAME_ACHIEVEMENT[itemID];
    descText = DESC_ACHIEVEMENT[itemID];
    image = NULL;
    ttlTextRect = GRect(3, -6, size.w-3, size.h);
    topTextRect = GRect(3, 16, size.w-3, size.h-22);
  } else {
    return;
  }

  GTextAlignment algn = GTextAlignmentLeft;
  if (row%2==0) algn = GTextAlignmentRight;

  if (selected) backColor = MENU_BACK_GREEN_SELECT;
  else if (row%2==0) backColor = MENU_BACK_GREEN_EVEN;
  else backColor = MENU_BACK_GREEN_ODD;

  graphics_context_set_fill_color(ctx, backColor);
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);

  graphics_draw_text(ctx, nameText, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, algn, NULL);
  graphics_draw_text(ctx, descText, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, algn, NULL);
  // Image
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  drawBitmap(ctx, image, imageRect);

}

///
/// SETUP
///

void settings_sub_window_load(Window* parentWindow) {

  // Now we prepare to initialize the menu layer
  s_settingsSubLayer = menu_layer_create(layer_get_frame(window_get_root_layer(parentWindow)));
  const int context = *((int*) window_get_user_data(parentWindow));
  APP_LOG(APP_LOG_LEVEL_DEBUG,"StngSubW%iLOAD", context);

  if (context == UNIQUE_CONTEXT_ID) initPrestigeWindowRes();
  // Create the menu layer
  menu_layer_set_callbacks(s_settingsSubLayer, window_get_user_data(parentWindow), (MenuLayerCallbacks){
    .get_num_sections = settings_sub_menu_get_num_sections_callback,
    .get_num_rows = settings_sub_menu_get_num_rows_callback,
    .get_cell_height = settings_sub_menu_get_cell_height_callback,
    .get_header_height = settings_sub_menu_get_header_height_callback,
    .draw_header = settings_sub_menu_draw_header_callback,
    .draw_row = settings_sub_menu_draw_row_callback,
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_normal_colors(s_settingsSubLayer, MENU_BACK_GREEN_ODD, GColorBlack);
  menu_layer_set_click_config_onto_window(s_settingsSubLayer, parentWindow);
  layer_add_child(window_get_root_layer(parentWindow), menu_layer_get_layer(s_settingsSubLayer));
}

void settings_sub_window_unload(Window* parentWindow) {
  const int context = *((int*) window_get_user_data(parentWindow));
  APP_LOG(APP_LOG_LEVEL_DEBUG,"StngSubW%iDEST", context);
  menu_layer_destroy(s_settingsSubLayer);
  s_settingsSubLayer = NULL;
  if (context == UNIQUE_CONTEXT_ID) deinitPrestigeWindowRes();
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
  APP_LOG(APP_LOG_LEVEL_DEBUG,"StngWLd");

  // Now we prepare to initialize the menu layer
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect bounds = layer_get_frame(window_layer);

  // Get the current settings and the list of what is unlocked
  loadUserSettings();

  // Create the menu layer
  s_settings_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_settings_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = settings_get_num_sections_callback,
    .get_num_rows = settings_get_num_rows_callback,
    .get_cell_height = settings_get_cell_height_callback,
    .get_header_height = settings_get_header_height_callback,
    .draw_header = settings_draw_header_callback,
    .draw_row = settings_draw_row_callback,
    .select_click = settings_select_callback,
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_normal_colors(s_settings_layer, MENU_BACK_BLUE_EVEN, GColorBlack);
  menu_layer_set_click_config_onto_window(s_settings_layer, parentWindow);
  menu_layer_pad_bottom_enable(s_settings_layer, false);
  layer_add_child(window_layer, menu_layer_get_layer(s_settings_layer));

  // Setup sub-windows that we might want to jump to
  createSettingsSubWin(&s_chevo_window, &s_chevo_context);
  createSettingsSubWin(&s_unique_window, &s_unique_context);
}

void settings_window_unload() {
  APP_LOG(APP_LOG_LEVEL_DEBUG,"StngWinDstry");
  menu_layer_destroy(s_settings_layer);
  s_settings_layer = NULL;
  window_destroy(s_chevo_window);
  window_destroy(s_unique_window);
  s_chevo_window = NULL;
  s_unique_window = NULL;
  APP_LOG(APP_LOG_LEVEL_DEBUG,"StngWinDstryDone");
}
