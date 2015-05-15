#include <pebble.h>
#include "settingsMenu.h"
#include "constants.h"
#include "persistence.h"
#include "timeStore.h"
#include "sellMenu.h"


#define CHEVO_CONTEXT_ID 0
#define UNIQUE_CONTEXT_ID 1

#define STAT_SECTION_ID 0
#define CHEVO_SECTION_ID 1
#define UNLOCK_SECTION_ID 2
#define SETTINGS_SECTION_ID 3

#define NUM_SETTINGS_MENU_SECTIONS 4
//
#define NUM_STAT_ROWS 3
#define NUM_CHEVO_ROWS 2
#define NUM_UNLOCK_ROWS 5
#define NUM_SETTINGS_ROWS 6

#define SETTINGS_CELL_HEIGHT 57

#define RESTART_COUNTDOWN 8

static MenuLayer* s_settings_layer;
static MenuLayer* s_chevo_layer;
static MenuLayer* s_unique_layer;

static Window* s_chevo_window;
static Window* s_unique_window;

static int s_chevo_context = CHEVO_CONTEXT_ID;
static int s_unique_context = UNIQUE_CONTEXT_ID;

static char tempBuffer[TEXT_BUFFER_SIZE];

static uint8_t s_timeDisplay = 0;
static uint8_t s_buildingDisplay = 0;
static uint8_t s_itemDisplay = 0;
static uint8_t s_restartCheck = RESTART_COUNTDOWN;

#define SETTINGS_UPDATEF_ID 0
#define SETTINGS_ANIMATE_ID 1
#define SETTINGS_CELSIUS_ID 2
#define SETTINGS_QUIETS_ID 3
#define SETTINGS_QUIETE_ID 4
#define SETTINGS_RESET_ID 5

// Hold unlockable settings and which are currently locked
#define UNLOCK_TECH_ID 0
#define UNLOCK_LIGHT_ID 1
#define UNLOCK_FONT_ID 2
#define UNLOCK_VIBE_ID 3
#define UNLOCK_COLOUR_ID 4

static uint8_t s_unlockSetting[NUM_UNLOCK_ROWS];
static uint8_t s_unlockedTo[NUM_UNLOCK_ROWS]; // How far has the user actually unlocked?

void loadUserSettings() {
  s_unlockSetting[UNLOCK_TECH_ID]   = getUserSetting(SETTING_TECH);
  s_unlockSetting[UNLOCK_LIGHT_ID]  = getUserSetting(SETTING_LIGHT);
  s_unlockSetting[UNLOCK_FONT_ID]   = getUserSetting(SETTING_TYPE);
  s_unlockSetting[UNLOCK_VIBE_ID]   = getUserSetting(SETTING_VIBE);
  s_unlockSetting[UNLOCK_COLOUR_ID] = getUserSetting(SETTING_COLOUR);

  s_unlockedTo[UNLOCK_TECH_ID]   = getUserOwnsUpgrades(WATCHER_ID, WATCHER_TECH);
  s_unlockedTo[UNLOCK_LIGHT_ID]  = getUserOwnsUpgrades(WATCHER_ID, WATCHER_LIGHT);
  s_unlockedTo[UNLOCK_FONT_ID]   = getUserOwnsUpgrades(WATCHER_ID, WATCHER_FONT);
  s_unlockedTo[UNLOCK_VIBE_ID]   = getUserOwnsUpgrades(WATCHER_ID, WATCHER_VIBE);
  s_unlockedTo[UNLOCK_COLOUR_ID] = getUserOwnsUpgrades(WATCHER_ID, WATCHER_COLOUR);
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
  	case SETTINGS_SECTION_ID: return NUM_SETTINGS_ROWS;
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
  } else if (section_index == SETTINGS_SECTION_ID) {
    strcpy(tempBuffer, "SETTINGS");
    graphics_context_set_fill_color(ctx, MENU_BACK_RED_ODD);
  }
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);
  menu_cell_basic_header_draw(ctx, cell_layer, tempBuffer);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, GPoint(0,0), GPoint(size.w, 0) );
}

static int16_t settings_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  switch(cell_index->section) {
    case STAT_SECTION_ID: return SETTINGS_CELL_HEIGHT;
    case CHEVO_SECTION_ID: return 32;
    case SETTINGS_SECTION_ID: case UNLOCK_SECTION_ID: return 44;
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
  else if (section == SETTINGS_SECTION_ID && selected   ) backColor = MENU_BACK_RED_SELECT;
  else if (section == SETTINGS_SECTION_ID && row%2 == 0 ) backColor = MENU_BACK_RED_EVEN;
  else if (section == SETTINGS_SECTION_ID               ) backColor = MENU_BACK_RED_ODD;
  graphics_context_set_fill_color(ctx, backColor);
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);

  GRect ttlTextRect = GRect(0, 0,  size.w, size.h);
  GRect topTextRect = GRect(0, 22, size.w, size.h-22);
  GRect botTextRect = GRect(0, 33, size.w, size.h-33);

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
        strcpy(titleText, "TOTAL Time >");
        timeToString(getUserTime(), subText1, TEXT_LARGE_BUFFER_SIZE, false);
      } else if (s_timeDisplay == 1) {
        strcpy(titleText, "ALLTime >");
        timeToString(getUserTotalTime(), subText1, TEXT_LARGE_BUFFER_SIZE, false);
      }


    } else if (row == 1) { // DISPLAY BUILDINGS


      if (s_buildingDisplay == 0) {
        strcpy(titleText, "REFINERY Bonus >");
        timeToString(getTimePerMin() - SEC_IN_MIN, subText1, TEXT_LARGE_BUFFER_SIZE, false);
      } else if (s_buildingDisplay == 1) {
        strcpy(titleText, "TANK Capacity >");
        timeToString(getTankCapacity(), subText1, TEXT_LARGE_BUFFER_SIZE, false);
      } else if (s_buildingDisplay == 2) {
        strcpy(titleText, "SIEVE Chance >");
        strcpy(subText1, "TODO");
      } else if (s_buildingDisplay == 3) {
        strcpy(titleText, "WATCHER Chance >");
        strcpy(subText1, "TODO");
      }

    } else if (row == 2) { // DISPLAY ITEMS

      if      (s_itemDisplay == COMMON_ID)    strcpy(titleText, "COMMON Items >");
      else if (s_itemDisplay == MAGIC_ID)     strcpy(titleText, "MAGIC Items >");
      else if (s_itemDisplay == RARE_ID)      strcpy(titleText, "RARE Items >");
      else if (s_itemDisplay == EPIC_ID)      strcpy(titleText, "EPIC Items >");
      else if (s_itemDisplay == LEGENDARY_ID) strcpy(titleText, "LEGENDARIES >");
      snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "OWNED: %i", (int)getUserTotalItems(s_itemDisplay) );
      strcpy(subText2, "VALUE:");
      timeToString(currentCategorySellPrice(s_itemDisplay), tempBuffer, TEXT_BUFFER_SIZE, true);
      strcat(subText2, tempBuffer);
      if (s_itemDisplay == LEGENDARY_ID) strcpy(subText2, "VALUE: Timeless"); // Cannot sell legendaries

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

    if (row == UNLOCK_TECH_ID) {

      strcpy(titleText, "TECH Addons");
      snprintf(subText1, TEXT_BUFFER_SIZE, "%i/%i] ", (int)setting+1, TECH_MAX);
      if      (setting > lock) strcat(subText1, "LOCKED");
      else if (setting == TECH_NONE)    strcat(subText1, "NONE");
      else if (setting == TECH_BATTERY) strcat(subText1, "BATTERY");
      else if (setting == TECH_MONTH)   strcat(subText1, "BATTERY+MONTH");
      else if (setting == TECH_WEATHER) strcat(subText1, "BATTERY+MONTH+WTHR");

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


  } else if (section == SETTINGS_SECTION_ID) {


    if (row == SETTINGS_UPDATEF_ID) { // Seconds

      strcpy(titleText, "UPDATE Freq.");
      if (getUserOpt(OPT_SHOW_SECONDS)) strcpy(subText1, "1/2] SECONDS");
      else strcpy(subText1, "2/2] MINUTES");

    } else if (row == SETTINGS_ANIMATE_ID) { // Animate

      strcpy(titleText, "ANIMATIONS");
      if (getUserOpt(OPT_ANIMATE)) strcpy(subText1, "ON");
      else strcpy(subText1, "OFF");

    } else if (row == SETTINGS_CELSIUS_ID) { // Temp

      strcpy(titleText, "TEMPERATURE");
      if (getUserOpt(OPT_CELSIUS)) strcpy(subText1, "CELSIUS");
      else strcpy(subText1, "FARENHEIT");

    } else if (row == SETTINGS_QUIETS_ID) { // Quite start

      strcpy(titleText, "QUIET Start");
      snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "Don't notify from %ih", (int)getUserSetting(SETTING_ZZZ_START) );

    } else if (row == SETTINGS_QUIETE_ID) { // Quiet end

      strcpy(titleText, "QUIET End");
      snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "... until %ih", (int)getUserSetting(SETTING_ZZZ_END) );

    } else if (row == SETTINGS_RESET_ID) { // new game

      strcpy(titleText, "RESET Game!");
      if (s_restartCheck == 8) strcpy(subText1, "Start again?");
      else if (s_restartCheck == 7) strcpy(subText1, "Are you sure?");
      else if (s_restartCheck == 6) strcpy(subText1, "Really really sure??");
      else if (s_restartCheck > 0) snprintf(subText1, TEXT_BUFFER_SIZE, "Click %i more times!", (int)s_restartCheck);
      else if (s_restartCheck == 0) strcpy(subText1, "GAME RESET!");

    }

  } else {
    return;
  }

  graphics_draw_text(ctx, titleText, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, subText1, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, subText2, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), botTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, GPoint(0,0), GPoint(size.w, 0) );
}

static void settings_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  const uint16_t section = cell_index->section;
  const uint16_t row = cell_index->row;
  // Stat section
  if (section == STAT_SECTION_ID) {

    if (row == 0) {
      if (++s_timeDisplay == 2) s_timeDisplay = 0;
    } else if (row == 1) {
      if (++s_buildingDisplay == 4) s_buildingDisplay = 0;
    } else if (row == 2) {
      // Only show for categories where we have items - no spoils
      while (++s_itemDisplay < 5 && getUserTotalItems(s_itemDisplay) == 0) {}
      if (s_itemDisplay == 5) s_itemDisplay = 0;
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

  } else if (section == SETTINGS_SECTION_ID) {

    if (row == SETTINGS_ANIMATE_ID) { // Animate
      flipUserOpt(OPT_ANIMATE);
    } else if (row == SETTINGS_UPDATEF_ID) { // Seconds
      flipUserOpt(OPT_SHOW_SECONDS);
    } else if (row == SETTINGS_CELSIUS_ID) { // Temp
      flipUserOpt(OPT_CELSIUS);
    } else if (row == SETTINGS_QUIETS_ID) { // Quite start
      incrementUserSetting(SETTING_ZZZ_START);
    } else if (row == SETTINGS_QUIETE_ID) { // Quiet end
      incrementUserSetting(SETTING_ZZZ_END);
    } else if (row == 8) { // new game
      if (--s_restartCheck == 0) resetUserData();
    }

  }
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

///
/// SETTINGS WINDOW CALLBACKS
///

static uint16_t settings_sub_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return 1; }

static uint16_t settings_sub_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  const int context = *((int*)data);
  if (context == CHEVO_CONTEXT_ID) return getTotalChevos();
  else if (context == UNIQUE_CONTEXT_ID) return getUserTotalItems(LEGENDARY_ID);
  return 0;
}

static int16_t settings_sub_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t settings_sub_menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) { return MENU_CELL_HEIGHT; }

static void settings_sub_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  const int context = *((int*)data);
  const GSize size = layer_get_frame(cell_layer).size;
  if (context == CHEVO_CONTEXT_ID)       menu_cell_basic_header_draw(ctx, cell_layer, "ACHIEVEMENTS");
  else if (context == UNIQUE_CONTEXT_ID) menu_cell_basic_header_draw(ctx, cell_layer, "LEGENDARIES");

}

static void settings_sub_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  const int context = *((int*)data);
  const GSize size = layer_get_frame(cell_layer).size;
  const uint16_t row = cell_index->row;
  const bool selected = menu_cell_layer_is_highlighted(cell_layer);

  int8_t itemID;
  const char* nameText;
  const char* descText;
  if (context == UNIQUE_CONTEXT_ID) {
    itemID = getItemIDFromRow(LEGENDARY_ID, row);
    if (itemID == -1) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "NoItem4SettingRow");
      return;
    }
    nameText = NAME_LEGENDARY[itemID];
    descText = DESC_LEGENDARY[itemID];
  } else if (context == CHEVO_CONTEXT_ID) { // CHEVO
    itemID = getAchievementIDFromRow(row);
    if (itemID == -1) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "NoItem4SettingRow");
      return;
    }
    nameText = NAME_ACHIEVEMENT[itemID];
    descText = NAME_ACHIEVEMENT[itemID];
  } else {
    return;
  }

  GColor backColor;
  if (selected) backColor = MENU_BACK_YELLOW_SELECT;
  else if (row%2==0) backColor = MENU_BACK_YELLOW_EVEN;
  else backColor = MENU_BACK_YELLOW_ODD;
  graphics_context_set_fill_color(ctx, backColor);
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);

  GRect ttlTextRect = GRect(MENU_X_OFFSET, -6, size.w-MENU_X_OFFSET, size.h);
  GRect topTextRect = GRect(MENU_X_OFFSET, 16, size.w-MENU_X_OFFSET, size.h-22);
  graphics_draw_text(ctx, nameText, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), ttlTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, descText, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), topTextRect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  // Image
  GRect imageRect = GRect(3, 4,  22, 36);
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  //graphics_draw_bitmap_in_rect(ctx, getItemImage(LEGENDARY_ID, itemID), imageRect);

}

static void settings_sub_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
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
  APP_LOG(APP_LOG_LEVEL_DEBUG,"SETTINGS SUB-WIN %i LOAD", context);

  new_layer = menu_layer_create(bounds);
  if (context == CHEVO_CONTEXT_ID)       s_chevo_layer = new_layer;
  else if (context == UNIQUE_CONTEXT_ID) s_unique_layer = new_layer;
  // Create the menu layer
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
  APP_LOG(APP_LOG_LEVEL_DEBUG,"SETTINGS SUB-WIN %i DESTROY", context);
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
  APP_LOG(APP_LOG_LEVEL_DEBUG,"SETTINGS WIN LOAD");

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
  menu_layer_set_click_config_onto_window(s_settings_layer, parentWindow);
  layer_add_child(window_layer, menu_layer_get_layer(s_settings_layer));

  // Setup sub-windows that we might want to jump to
  createSettingsSubWin(&s_chevo_window, &s_chevo_context);
  createSettingsSubWin(&s_unique_window, &s_unique_context);
  s_restartCheck = RESTART_COUNTDOWN;

}

void settings_window_unload() {
  APP_LOG(APP_LOG_LEVEL_DEBUG,"SETTINGS WIN DESTROY");
  menu_layer_destroy(s_settings_layer);
  window_destroy(s_chevo_window);
  window_destroy(s_unique_window);
}
