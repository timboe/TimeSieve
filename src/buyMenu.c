#include <pebble.h>
#include "buyMenu.h"
#include "constants.h"
#include "persistence.h"
  
#define NUM_BUY_MENU_SECTIONS 1
#define NUM_BUY_MENU_ITEMS 4
  
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

/// 
/// MAIN BUY WINDOW CALLBACKS
///

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_BUY_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  if (section_index == 0) return NUM_BUY_MENU_ITEMS;
  return 0;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  //if (section_index == 0) return 0; // No header for the basic buy menu
  //return 0;
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  //if (section_index == 0) {} // Do nothing
  menu_cell_basic_header_draw(ctx, cell_layer, "Buy Upgrades");
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  GSize size = layer_get_frame(cell_layer).size;
  bool selected = menu_cell_layer_is_highlighted(cell_layer);
  if (selected)                      graphics_context_set_fill_color(ctx, BUY_MENU_BACK_COLOUR_SELECT);
  else if (cell_index->row % 2 == 1) graphics_context_set_fill_color(ctx, BUY_MENU_BACK_COLOUR_ODD);
  else                               graphics_context_set_fill_color(ctx, BUY_MENU_BACK_COLOUR_EVEN);
  graphics_fill_rect(ctx, GRect(0, 0, size.w, size.h), 0, GCornersAll);
  if (cell_index->section == 0) {
    if (cell_index->row == 0) { 
      menu_cell_basic_draw(ctx, cell_layer, "REFINERY Upgrades", "Get more liquid time", NULL);
    } else if (cell_index->row == 1) {
      menu_cell_basic_draw(ctx, cell_layer, "TANK Upgrades", "Store more liquid time", NULL);
    } else if (cell_index->row == 2) {
      menu_cell_basic_draw(ctx, cell_layer, "SIEVE Upgrades", "Find more treasures", NULL);
    } else if (cell_index->row ==3) {
      //menu_cell_title_draw(ctx, cell_layer, "Final Item");
      menu_cell_basic_draw(ctx, cell_layer, "WATCHERS", "Collect more treasure", NULL);
    }
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  if (cell_index->row == 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "PUSH REFINARY ONTO STACK");
    window_stack_push(s_refinery_window, true);
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

static uint16_t refinery_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) { return 1; }

static uint16_t refinery_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return N_REFINERY_UPGRADES; }

static int16_t refinery_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) { return MENU_CELL_BASIC_HEADER_HEIGHT; }

static void refinery_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "REFINARY Upgrades");
}

static void refinery_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  
  
  //Get name
  const unsigned location = cell_index->row;
  const char* upgradeName = NAME_REFINERY[location];
  // Get owned
  const uint16_t owned = getUserOwnsUpgrades(REFINERY_ID, location);
  
  bool display = true;
  if (owned == 0) {
    // Can I afford this? Only display if sp
    
  }
  
  
  
  // Determine which section we're going to draw in
    
    
  if (cell_index->row == 0) {
    menu_cell_basic_draw(ctx, cell_layer, "A", "Get more liquid time", NULL);
  } else if (cell_index->row == 1) {
    menu_cell_basic_draw(ctx, cell_layer, "B", "Store more liquid time", NULL);
  } else if (cell_index->row == 2) {
    menu_cell_basic_draw(ctx, cell_layer, "C", "Find more treasures", NULL);
  } else {
    menu_cell_basic_draw(ctx, cell_layer, "D", "Collect more treasure", NULL);
  }
}

static void refinery_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

}

/// 
/// SETUP
///

void refinery_window_load(Window* parentWindow) {
  // Now we prepare to initialize the menu layer
  Layer* window_layer = window_get_root_layer(parentWindow);
  GRect bounds = layer_get_frame(window_layer);
  // Create the menu layer
  s_refinery_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_refinery_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = refinery_menu_get_num_sections_callback,
    .get_num_rows = refinery_menu_get_num_rows_callback,
    .get_header_height = refinery_menu_get_header_height_callback,
    .draw_header = refinery_menu_draw_header_callback,
    .draw_row = refinery_menu_draw_row_callback,
    .select_click = refinery_menu_select_callback,
  });
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_refinery_layer, parentWindow);
  layer_add_child(window_layer, menu_layer_get_layer(s_refinery_layer));
}

void refinery_window_unload() {
  menu_layer_destroy(s_refinery_layer);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
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
  // Create the buy window 
  s_refinery_window = window_create();
  window_set_window_handlers(s_refinery_window, (WindowHandlers) {
    .load = refinery_window_load,
    .unload = refinery_window_unload
  });  

}

void buy_window_unload() {
  menu_layer_destroy(s_menu_layer);
  
  // Destroy sub-windows
  window_destroy(s_refinery_window);
}