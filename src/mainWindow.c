#include <pebble.h>
#include "mainWindow.h"
#include "clock.h"
#include "timeSieve.h"
#include "timeTank.h"
#include "timeStore.h"
#include "buyMenu.h"
#include "settingsMenu.h"
#include "sellMenu.h"

// Static pointer to primary windows
static Window* s_main_window;
static Window* s_buy_window;
static Window* s_settings_window;

// Main window initialisation
void main_window_load(Window *window) {
  create_clock_layer(window);
  create_timeSieve_layer(window);
  create_timeTank_layer(window);
}

// Main window destructiom
void main_window_unload(Window *window) {
  // Destroy output TextLayer
  destroy_clock_layer();
  destroy_timeSieve_layer();
  destroy_timeTank_layer();
}

// For the main window
void main_window_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  Window* window = (Window*) context;
  ButtonId _button = click_recognizer_get_button_id(recognizer);
  if (BUTTON_ID_DOWN == _button) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed the down button.");
    addTime(60); 
    update_timeTank_layer();
    animCallback();
  } else if (BUTTON_ID_UP == _button) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed the buy window.");
    window_stack_push(s_buy_window, true);
  } else if (BUTTON_ID_SELECT == _button) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "middle button : just pushed settings");
    window_stack_push(s_settings_window, true);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "unknown button.");

  }
}

void click_config_provider(Window *window) {
 // single click / repeat-on-hold config:
  window_single_click_subscribe(BUTTON_ID_DOWN|BUTTON_ID_UP|BUTTON_ID_SELECT, main_window_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, main_window_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, main_window_single_click_handler);
//   window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_single_click_handler);
//   // multi click config:
//   window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);
//   // long click config:
//   window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

void init_mainWindow() {
  // Create main window and set load/unload
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_fullscreen(s_main_window, true);
  window_set_click_config_provider(s_main_window, (ClickConfigProvider) click_config_provider);
  
  // Create the buy window 
  s_buy_window = window_create();
  window_set_background_color(s_buy_window, MENU_BACK_GREEN_ODD);
  window_set_window_handlers(s_buy_window, (WindowHandlers) {
    .load = buy_window_load,
    .unload = buy_window_unload
  });  

  // Create the settings window 
  s_settings_window = window_create();
  window_set_window_handlers(s_settings_window, (WindowHandlers) {
    .load = settings_window_load,
    .unload = settings_window_unload
  });  

  window_stack_push(s_main_window, true);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed the main window.");
}

void destroy_mainWindow() {
  window_destroy(s_main_window);
  window_destroy(s_buy_window);
  window_destroy(s_settings_window);
}