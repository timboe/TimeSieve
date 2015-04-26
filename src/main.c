#include <pebble.h>
#include "clock.h"
#include "timeSieve.h"
#include "timeTank.h"
#include "timeStore.h"
#include "buyMenu.h" 
#include "resources.h"
#include "persistence.h"
  
// Static pointer to main windows
static Window* s_main_window;
static Window* s_buy_window;

// Main window initialisation
static void main_window_load(Window *window) {
  create_clock_layer(window);
  create_timeSieve_layer(window);
  create_timeTank_layer(window);
}

// Main window destructiom
static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  destroy_clock_layer();
  destroy_timeSieve_layer();
  destroy_timeTank_layer();
}

// For the main window
void menu_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  Window* window = (Window*) context;
  ButtonId _button = click_recognizer_get_button_id(recognizer);
  if (BUTTON_ID_DOWN == _button) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed the down button.");
    s_userLTime += 60*6; 
    update_timeTank_layer();
  } else if (BUTTON_ID_UP == _button) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed the buy window.");
    window_stack_push(s_buy_window, true);
  } else if (BUTTON_ID_SELECT == _button) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed the middle button.");
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "unknown button.");

  }
}

void click_config_provider(Window *window) {
 // single click / repeat-on-hold config:
  window_single_click_subscribe(BUTTON_ID_DOWN|BUTTON_ID_UP|BUTTON_ID_SELECT, menu_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, menu_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, menu_single_click_handler);
//   window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_single_click_handler);
//   // multi click config:
//   window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);
//   // long click config:
//   window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

// Prog initialisation
void handle_init(void) {
  
  srand(time(NULL));
  init_resources();
  init_persistence();
  init_timeStore(); // neesds persistence
  
  // Create main window and set load/unload
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_fullscreen(s_main_window, true);
  // Set click provider
  window_set_click_config_provider(s_main_window, (ClickConfigProvider) click_config_provider);
  
  // Create the buy window 
  s_buy_window = window_create();
  window_set_background_color(s_buy_window, BUY_MENU_BACK_COLOUR_ODD);
  window_set_window_handlers(s_buy_window, (WindowHandlers) {
    .load = buy_window_load,
    .unload = buy_window_unload
  });  


  window_stack_push(s_main_window, true);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed the main window.");
}

void handle_deinit(void) {
	// Destroy the window
	window_destroy(s_main_window);
  window_destroy(s_buy_window);
  // Destroy still loaded resources
  destroy_resources();
  destroy_timeStore();
  destroy_persistence();
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
