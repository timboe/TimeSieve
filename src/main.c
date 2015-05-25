#include <pebble.h>
#include "timeStore.h"
#include "mainWindow.h"
#include "resources.h"
#include "persistence.h"
#include "clock.h"
#include "splash.h"
#include "constants.h"

/**
 * These two opertaions may take appreciable time if the game has not been loaded in a while
 **/
void doTimeConsuming(void* data) {
  init_timeStore(); // Buffer upgrade prices, item prices, item probabilities
  doCatchup(); // Give user time and items missed while programme not running
}

// Prog initialisation
void handleInit(void) {
  init_persistence(); // Load user save
  initGlobalRes(); // Resources used throughout app
  if (getNeedSplashScreen()) {
    showSplash(); // Show intro screen while we do time consuming calcs
    app_timer_register(5, doTimeConsuming, NULL); // Schedule for in 5ms, after splash window is rendered
  } else {
    doTimeConsuming(NULL); // Won't take long - do it now
    init_mainWindow(); // Needs to be envoked manually here, won't be called by the splash screen.
  }
  if (IS_DEBUG) {
    light_enable(1);
    memRep((void*)1);
  }
}

void handleDestroy(void) {
  // Goodbye
  destroy_mainWindow();
  destroy_timeStore();
  destroy_persistence(); // Save!
  destroyGlobalRes();
}

int main(void) {
  handleInit();
	app_event_loop();
	handleDestroy();
  return 0;
}
