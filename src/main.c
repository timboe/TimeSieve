#include <pebble.h>
#include "timeStore.h"
#include "mainWindow.h" 
#include "resources.h"
#include "persistence.h"
  
// Prog initialisation
void handle_init(void) {
  srand(time(NULL)); // init random number gen
  light_enable(1); // TESTING
  init_resources(); // TESTING - to move to where used
  init_persistence(); // Load user save
  init_timeStore(); // Buffer upgrade prices
  init_mainWindow(); // Create and push to the stack the main window
  DEVMODE();
  update_tick_handler(); // Get 1s or 1m callbacks
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
  destroy_mainWindow();
  destroy_resources();
  destroy_timeStore();
  destroy_persistence();
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
