#include <pebble.h>
#include "persistence.h"

static struct userData_v1* s_userData;  
  
void init_persistence() {

  //Creaet new
  s_userData = malloc(sizeof(struct userData_v1));
  // Zero evertyhing
  memset(&s_userData, 0, sizeof(struct userData_v1));
  
  // set some defaults
  setUserTime(30);
  setUserTimeCapacity(600);
}

void destroy_persistence() { // Save
  free(s_userData);
  s_userData = 0;
}

uint16_t getUserOwnsUpgrades(const unsigned typeID, const unsigned resourceID) {
  if (resourceID >= MAX_UPGRADES) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "getUserOwnsUpgrades resourceID overflow");
    return 0;
  }
  if (typeID == REFINERY_ID) {
    return s_userData->refineriesOwned[resourceID];
  } else if (typeID == TANK_ID) {
    return s_userData->tanksOwned[resourceID];
  } else if (typeID == SIEVE_ID) {
    return s_userData->sievesOwned[resourceID];
  } else if (typeID == WATCHER_ID) {
    return s_userData->watchersOwned[resourceID];
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "getUserOwnsUpgrades typeID unknown");
  return 0;
}

void setUserTime(uint64_t newTime) {
  s_userData->currentTime = newTime;
}

void setUserTimeCapacity(uint64_t newTime) {
  s_userData->timeCapacity = newTime;
}

uint64_t getUserTime() {
  return s_userData->currentTime;
}

uint64_t getUserTimeCapacity() {
  return s_userData->timeCapacity;
}