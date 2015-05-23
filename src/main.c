#include <pebble.h>
#include "timeStore.h"
#include "mainWindow.h"
#include "resources.h"
#include "persistence.h"
#include "clock.h"
#include "splash.h"

// Prog initialisation
void handleInit(void) {
  srand(time(NULL)); // init random number gen
  init_persistence(); // Load user save
  init_timeStore(); // Buffer upgrade prices, item prices, item probabilities
  doCatchup(); // Give user time and items missed while programme not running
  showSplash();
  light_enable(1);
}

void handleDeinit(void) {
  // Goodbye
  destroy_mainWindow();
  destroy_timeStore();
  destroy_persistence(); // Save!
}

int main(void) {
  handleInit();
	app_event_loop();
	handleDeinit();
}
