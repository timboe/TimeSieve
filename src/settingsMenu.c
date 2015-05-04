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
//
#define NUM_STAT_ROWS 3
#define NUM_CHEVO_ROWS 2
#define NUM_SETTINGS_ROWS 9

#define SETTINGS_CELL_HEIGHT 57

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
static uint8_t s_restartCheck = 8;

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
    graphics_context_set_fill_color(ctx, MENU_BACK_YELLOW_ODD);
  } else if (section_index == CHEVO_SECTION_ID) {
    strcpy(tempBuffer, "ACHIEVEMENTS");
    graphics_context_set_fill_color(ctx, MENU_BACK_GREEN_ODD);
  } else if (section_index == SETTINGS_SECTION_ID) {
    strcpy(tempBuffer, "SETTINGS");
    graphics_context_set_fill_color(ctx, MENU_BACK_BLUE_ODD);
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
    case SETTINGS_SECTION_ID: return 44;
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
  else if (section == SETTINGS_SECTION_ID && selected   ) backColor = MENU_BACK_BLUE_SELECT;
  else if (section == SETTINGS_SECTION_ID && row%2 == 0 ) backColor = MENU_BACK_BLUE_EVEN;
  else if (section == SETTINGS_SECTION_ID               ) backColor = MENU_BACK_BLUE_ODD;
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

      if (s_itemDisplay == 0) {
        graphics_context_set_text_color(ctx, COLOUR_COMMON);
        strcpy(titleText, "COMMON Items >");
        snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "OWNED: %i", (int)getUserTotalItems(COMMON_ID) );    
        strcpy(subText2, "VALUE: TODO");        
      } else if (s_itemDisplay == 1) {
        graphics_context_set_text_color(ctx, COLOUR_MAGIC);
        strcpy(titleText, "MAGIC Items >");  
        snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "OWNED: %i", (int)getUserTotalItems(MAGIC_ID) );  
        strcpy(subText2, "VALUE: TODO");     
      } else if (s_itemDisplay == 2) {
        graphics_context_set_text_color(ctx, COLOUR_RARE);
        strcpy(titleText, "RARE Items >");        
        snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "OWNED: %i", (int)getUserTotalItems(RARE_ID) );   
        strcpy(subText2, "VALUE: TODO");    
      } else if (s_itemDisplay == 3) {
        graphics_context_set_text_color(ctx, COLOUR_EPIC);
        strcpy(titleText, "EPIC Items >"); 
        snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "OWNED: %i", (int)getUserTotalItems(EPIC_ID) );    
        strcpy(subText2, "VALUE: TODO");   
      } else if (s_itemDisplay == 4) {
        graphics_context_set_text_color(ctx, COLOUR_LEGENDARY);
        strcpy(titleText, "LEGENDARIES >"); 
        snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "OWNED: %i", (int)getUserTotalItems(LEGENDARY_ID) );  
        strcpy(subText2, "VALUE: TODO");     
      }
      if (!selected) graphics_context_set_text_color(ctx, GColorBlack);

    }

  } else if (section == CHEVO_SECTION_ID) {

    if (row == 0) { // DISPLAY LEGENDARY
      strcpy(titleText, "LEGENDARIES");        
    } else if (row == 1) { // DISPLAY CHEVO
      strcpy(titleText, "ACHIEVEMENTS");        
    }

  } else if (section == SETTINGS_SECTION_ID) {

// Color
// Typeface
// Animate
// Seconds
// Light on treasure
// Vibe on treasure
// Quite start
// Quiet end
// new game


//TODO - many of these should be locked
    if (row == 0) { // Color
      strcpy(titleText, "COLOUR Theme"); 
      if      (getUserColorTheme() == PALETTE_BLUE)   strcpy(subText1, "BLUE---"); 
      else if (getUserColorTheme() == PALETTE_GREEN)  strcpy(subText1, "-GREEN--");
      else if (getUserColorTheme() == PALETTE_YELLOW) strcpy(subText1, "--YELLOW-"); 
      else if (getUserColorTheme() == PALETTE_RED)    strcpy(subText1, "---RED"); 
    } else if (row == 1) { // Typeface
      strcpy(titleText, "CLOCK Typeface");
      if      (getUserTypeSetting() == 0) strcpy(subText1, "0----"); 
      else if (getUserTypeSetting() == 1) strcpy(subText1, "-1---");
      else if (getUserTypeSetting() == 2) strcpy(subText1, "--2--"); 
      else if (getUserTypeSetting() == 3) strcpy(subText1, "---3-"); 
      else if (getUserTypeSetting() == 4) strcpy(subText1, "----4"); 
    } else if (row == 2) { // Animate
      strcpy(titleText, "ANIMATIONS"); 
      if (getUserOpt(OPT_ANIMATE)) strcpy(subText1, "ON-"); 
      else strcpy(subText1, "-OFF"); 
    } else if (row == 3) { // Seconds
      strcpy(titleText, "UPDATE Freq.");
      if (getUserOpt(OPT_SHOW_SECONDS)) strcpy(subText1, "-SECONDS"); 
      else strcpy(subText1, "MINUTES-");    
    } else if (row == 4) { // Light on treasure
      strcpy(titleText, "LIGHT Notify");
      if      (getUserLightSetting() == 0) strcpy(subText1, "OFF-----"); 
      else if (getUserLightSetting() == 1) strcpy(subText1, "-COMMON+----"); 
      else if (getUserLightSetting() == 2) strcpy(subText1, "--MAGIC+---"); 
      else if (getUserLightSetting() == 3) strcpy(subText1, "---RARE+--"); 
      else if (getUserLightSetting() == 4) strcpy(subText1, "----EPIC+-"); 
      else if (getUserLightSetting() == 5) strcpy(subText1, "----LEGENDARY");
    } else if (row == 5) { // Vibe on treasure
      strcpy(titleText, "VIBRATE Notify");
      if      (getUserVibrateSetting() == 0) strcpy(subText1, "OFF-----"); 
      else if (getUserVibrateSetting() == 1) strcpy(subText1, "-COMMON+----"); 
      else if (getUserVibrateSetting() == 2) strcpy(subText1, "--MAGIC+---"); 
      else if (getUserVibrateSetting() == 3) strcpy(subText1, "---RARE+--"); 
      else if (getUserVibrateSetting() == 4) strcpy(subText1, "----EPIC+-"); 
      else if (getUserVibrateSetting() == 5) strcpy(subText1, "----LEGENDARY");
    } else if (row == 6) { // Quite start
      strcpy(titleText, "QUIET Start");
      snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "Don't notify from %ih", (int)getUserZzzStartSetting() );    
    } else if (row == 7) { // Quiet end
      strcpy(titleText, "QUIET End");
      snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "... until %ih", (int)getUserZzzEndSetting() );    
    } else if (row == 8) { // new game
      strcpy(titleText, "RESET Game!");
      if (s_restartCheck == 8) strcpy(subText1, "Start again?");
      else if (s_restartCheck == 7) strcpy(subText1, "Are you sure?");
      else if (s_restartCheck == 6) strcpy(subText1, "Really really sure??");
      else if (s_restartCheck > 0) snprintf(subText1, TEXT_LARGE_BUFFER_SIZE, "Click %i more times!", (int)s_restartCheck); 
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
      if (++s_itemDisplay == 5) s_itemDisplay = 0;
    }
  
  } else if (section == CHEVO_SECTION_ID) {



  } else if (section == SETTINGS_SECTION_ID) {

    if (row == 0) { // Color
      incrementUserColorTheme();
    } else if (row == 1) { // Typeface
      incrementUserTypeSetting();
    } else if (row == 2) { // Animate
      flipUserOpt(OPT_ANIMATE);
    } else if (row == 3) { // Seconds
      flipUserOpt(OPT_SHOW_SECONDS);
    } else if (row == 4) { // Light on treasure
      incrementUserLightSetting();
    } else if (row == 5) { // Vibe on treasure
      incrementUserVibrateSetting();
    } else if (row == 6) { // Quite start
      incrementUserZzzStartSetting();
    } else if (row == 7) { // Quiet end
      incrementUserZzzEndSetting();
    } else if (row == 8) { // new game
      if (s_restartCheck > 0) --s_restartCheck;
      else resetUserData();
    }

  }
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

/// 
/// REFINARY BUY WINDOW CALLBACKS
///

static uint16_t settings_sub_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return 1; }

static uint16_t settings_sub_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { 
  return 0;
}

static int16_t settings_sub_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return 0; }

static int16_t settings_sub_menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) { return MENU_CELL_HEIGHT; }

static void settings_sub_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
}

static void settings_sub_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
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
  s_restartCheck = 8;

}

void settings_window_unload() {
  APP_LOG(APP_LOG_LEVEL_DEBUG,"SETTINGS WIN DESTROY");
  menu_layer_destroy(s_settings_layer);
  window_destroy(s_chevo_window);
  window_destroy(s_unique_window);
}