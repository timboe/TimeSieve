#include <pebble.h>
#include "timeStore.h"
#include "mainWindow.h" 
#include "resources.h"
#include "persistence.h"
  
// Prog initialisation
void handle_init(void) {
  
  srand(time(NULL));
  light_enable(1);
  init_resources();
  init_persistence();

  // testing
  APP_LOG(APP_LOG_LEVEL_INFO, "GIVE TEST TIME");
  setUserTime(17);
  
  init_timeStore(); // needs persistence
  init_mainWindow();
}

void handle_deinit(void) {
  destroy_mainWindow();
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
