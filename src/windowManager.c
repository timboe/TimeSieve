#include <pebble.h>
#include "windowManager.h"
#include "mainWindow.h"
#include "splash.h"
#include "settingsMenu.h"
#include "buyMenu.h"
#include "buyMenuSub.h"
#include "sellMenu.h"

static Window* s_splashWindow = NULL;
//
static Window* s_mainWindow = NULL;
//
static Window* s_settingsWindow = NULL;
static Window* s_chevoWindow = NULL;
static Window* s_uniqueWindow = NULL;
//
static Window* s_sellWindow = NULL;
//
static Window* s_buyWindow = NULL;
static Window* s_refineryWindow = NULL;
static Window* s_tankWindow = NULL;
static Window* s_employeeWindow = NULL;

static int s_refineryContext = REFINERY_ID;
static int s_tankContext = TANK_ID;
static int s_watcherContext = WATCHER_ID;
static int s_chevo_context = CHEVO_CONTEXT_ID;
static int s_unique_context = UNIQUE_CONTEXT_ID;


void initWindows() {

  s_splashWindow = window_create();
  window_set_window_handlers(s_splashWindow, (WindowHandlers) {
    .load = splashWindowLoad,
    .unload = splashWindowUnload
  });
  window_set_background_color(s_splashWindow, GColorDarkGray);
  window_set_click_config_provider(s_splashWindow, (ClickConfigProvider) splashClickConfProvider);

  /////////////////////////////////////////////////////

  s_mainWindow = window_create();
  window_set_window_handlers(s_mainWindow, (WindowHandlers) {
    .load = mainWindowLoad,
    .unload = mainWindowUnload
  });
  window_set_background_color(s_mainWindow, GColorBlack);
  window_set_click_config_provider(s_mainWindow, (ClickConfigProvider) mainWindowClickConfigProvider);

  // Create the menu windows
  s_buyWindow = window_create();
  window_set_window_handlers(s_buyWindow, (WindowHandlers) {
    .load = buyWindowLoad,
    .unload = buyWindowUnload
  });

  s_settingsWindow = window_create();
  window_set_window_handlers(s_settingsWindow, (WindowHandlers) {
    .load = settingsWindowLoad,
    .unload = settingsWindowUnload
  });

  s_sellWindow = window_create();
  window_set_window_handlers(s_sellWindow, (WindowHandlers) {
    .load = sellWindowLoad,
    .unload = sellWindowUnload
  });

/////////////////////////////////////////////////////

  s_refineryWindow = window_create();
  window_set_user_data(s_refineryWindow, &s_refineryContext);
  window_set_window_handlers(s_refineryWindow, (WindowHandlers) {
    .load = buySubWindowLoad,
    .unload = buySubWindowUnload
  });

  s_tankWindow = window_create();
  window_set_user_data(s_tankWindow, &s_tankContext);
  window_set_window_handlers(s_tankWindow, (WindowHandlers) {
    .load = buySubWindowLoad,
    .unload = buySubWindowUnload
  });

  s_employeeWindow = window_create();
  window_set_user_data(s_employeeWindow, &s_watcherContext);
  window_set_window_handlers(s_employeeWindow, (WindowHandlers) {
    .load = buySubWindowLoad,
    .unload = buySubWindowUnload
  });

/////////////////////////////////////////////////////

  s_chevoWindow = window_create();
  window_set_user_data(s_chevoWindow, &s_chevo_context);
  window_set_background_color(s_chevoWindow, MENU_BACK_BLUE_ODD);
  window_set_window_handlers(s_chevoWindow, (WindowHandlers) {
    .load = settingsSubWindowLoad,
    .unload = settingsSubWindowUnload
  });

  s_uniqueWindow = window_create();
  window_set_user_data(s_uniqueWindow, &s_unique_context);
  window_set_background_color(s_uniqueWindow, MENU_BACK_BLUE_ODD);
  window_set_window_handlers(s_uniqueWindow, (WindowHandlers) {
    .load = settingsSubWindowLoad,
    .unload = settingsSubWindowUnload
  });

}

void destroyWindows() {
  window_destroy(s_splashWindow);

  window_destroy(s_mainWindow);

  window_destroy(s_buyWindow);
  window_destroy(s_refineryWindow);
  window_destroy(s_tankWindow);
  window_destroy(s_employeeWindow);

  window_destroy(s_settingsWindow);
  window_destroy(s_uniqueWindow);
  window_destroy(s_chevoWindow);

  window_destroy(s_sellWindow);
}

void popWindow(bool anim) {
  window_stack_pop(anim);
}


void pushWindow(TIMESINK_WINDOWS_TYPE window, bool anim) {
  switch (window) {
    case WINDOW_SPLASH:      window_stack_push(s_splashWindow, anim); break;
    case WINDOW_MAIN:        window_stack_push(s_mainWindow, anim); break;
    case WINDOW_SELL:        window_stack_push(s_sellWindow, anim); break;
    case WINDOW_BUY:         window_stack_push(s_buyWindow, anim); break;
    case WINDOW_REFINARY:    window_stack_push(s_refineryWindow, anim); break;
    case WINDOW_TANK:        window_stack_push(s_tankWindow, anim); break;
    case WINDOW_EMPLOYEE:    window_stack_push(s_employeeWindow, anim); break;
    case WINDOW_SETTINGS:    window_stack_push(s_settingsWindow, anim); break;
    case WINDOW_ACHIEVEMENT: window_stack_push(s_chevoWindow, anim); break;
    case WINDOW_LEGENDARIES: window_stack_push(s_uniqueWindow, anim); break;
    default: APP_LOG(APP_LOG_LEVEL_DEBUG, "Unknown win %i", (int)window);
  }
}

void popPushWindow(TIMESINK_WINDOWS_TYPE window, bool anim) {
  popWindow(anim);
  pushWindow(window, anim);
}
