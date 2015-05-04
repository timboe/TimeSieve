#include <pebble.h>
#include "mainWindow.h"
#include "clock.h"
#include "timeSieve.h"
#include "timeTank.h"
#include "timeStore.h"
#include "buyMenu.h"
#include "settingsMenu.h"
#include "sellMenu.h"
#include "persistence.h"


void tick_handler(struct tm *tick_time, TimeUnits units_changed); //rm me later

// Static pointer to primary windows
static Window* s_main_window;
static Window* s_buy_window;
static Window* s_settings_window;
static Window* s_sell_window;

// Hold if the animation routine is still in progress and is requesting more frames.
static bool s_clockAnimRequest;
static bool s_sieveAnimRequest;
static bool s_tankAnimRequest;

/**
 * Function executes at 1/FPS while all areas are still requesting more frames.
 * Each anim function is expected to dirty any layers which need redrawing
 */ 
void animCallback(void* data) {
  if (s_tankAnimRequest)  s_tankAnimRequest  = tankAnimCallback();
  if (s_sieveAnimRequest) s_sieveAnimRequest = sieveAnimCallback();
  if (s_clockAnimRequest) s_clockAnimRequest = clockAnimCallback();
  if (s_clockAnimRequest || s_tankAnimRequest || s_clockAnimRequest) {
    app_timer_register(ANIM_DELAY, animCallback, NULL);
  }
}

/**
 * Call to start the per-min animation. Resets all amination and starts the callback.
 */
void animBegin() {
  clockAnimReset();
  sieveAnimReset();
  tankAnimReset();
  s_clockAnimRequest = true;
  s_sieveAnimRequest = true;
  s_tankAnimRequest = true;
  animCallback(NULL);
}

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
  ButtonId _button = click_recognizer_get_button_id(recognizer);

   if (BUTTON_ID_UP == _button) {
    window_stack_push(s_buy_window, true);
  } else if (BUTTON_ID_SELECT == _button) {
    window_stack_push(s_settings_window, true);
  } else if (BUTTON_ID_DOWN == _button) {
    window_stack_push(s_sell_window, true);
  }
  
}

void click_config_provider(Window *window) {
 // single click / repeat-on-hold config:
  window_single_click_subscribe(BUTTON_ID_DOWN|BUTTON_ID_UP|BUTTON_ID_SELECT, main_window_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, main_window_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, main_window_single_click_handler);
//   window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_single_click_handler);
//   window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);
//   window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

  // ADD USER TIME

  if (getUserOpt(OPT_SHOW_SECONDS) == true) { // seconds always pass
    // Update the user time, but only 1/60 of it (we give the remainder on the min mark)
    addTime( getTimePerMin() / SEC_IN_MIN );
    updateDisplayTime( getUserTime() ); // We don't animate the time if updating every sec
    update_timeTank_layer();
    updateClockLayer();
  }

  if ((units_changed & MINUTE_UNIT) != 0) { // Has 1m passed?
    APP_LOG(APP_LOG_LEVEL_INFO,"1m CALLED");
    if (getUserOpt(OPT_SHOW_SECONDS) == true) { // If we are doing per-sec updates then give the modulo remainder time
      addTime( getTimePerMin() % SEC_IN_MIN );
      updateDisplayTime( getUserTime() );
    } else { // Give the whole 1m allowance
      addTime( getTimePerMin() );
    } 
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO,"1m FAILED");
    return; // WARNING - if only 1s has passed we do not run anything below
  }

  // Begin animation?
  if (getUserOpt(OPT_ANIMATE) == true) { // do animation?
    animBegin();
  } else {
    updateDisplayTime( getUserTime() ); // Just set time value
    update_timeTank_layer(); // Show the user their new time
    updateClockLayer();  // Update the clock
  }

  // Found treasure?

  // Earned chevos?

  // Notification

}

void update_tick_handler() {
  if (getUserOpt(OPT_SHOW_SECONDS) == true) {
    tick_timer_service_subscribe(SECOND_UNIT|MINUTE_UNIT|HOUR_UNIT|DAY_UNIT|MONTH_UNIT|YEAR_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT|HOUR_UNIT|DAY_UNIT|MONTH_UNIT|YEAR_UNIT, tick_handler);
  }
}



void init_mainWindow() {
  // Create main window and set load/unload
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_click_config_provider(s_main_window, (ClickConfigProvider) click_config_provider);
  
  // Create the menu windows
  s_buy_window = window_create();
  window_set_background_color(s_buy_window, MENU_BACK_GREEN_ODD);
  window_set_window_handlers(s_buy_window, (WindowHandlers) {
    .load = buy_window_load,
    .unload = buy_window_unload
  });  

  s_settings_window = window_create();
  window_set_window_handlers(s_settings_window, (WindowHandlers) {
    .load = settings_window_load,
    .unload = settings_window_unload
  });  

  s_sell_window = window_create();
  window_set_window_handlers(s_sell_window, (WindowHandlers) {
    .load = sell_window_load,
    .unload = sell_window_unload
  });  

  if (s_main_window == NULL) APP_LOG(APP_LOG_LEVEL_DEBUG, "MainWin FAIL!");
  if (s_buy_window == NULL) APP_LOG(APP_LOG_LEVEL_DEBUG, "BuyWin FAIL!");
  if (s_settings_window == NULL) APP_LOG(APP_LOG_LEVEL_DEBUG, "SettingsWin FAIL!");
  if (s_sell_window == NULL) APP_LOG(APP_LOG_LEVEL_DEBUG, "SellWin FAIL!");

  window_stack_push(s_main_window, true);
}

void destroy_mainWindow() {
  window_destroy(s_main_window);
  window_destroy(s_buy_window);
  window_destroy(s_settings_window);
  window_destroy(s_sell_window);
}