#include <pebble.h>
#include "persistence.h"

static struct userData_v1* s_userData;  
  
void init_persistence() {

  // Check if first load
  int32_t version = 0;
  if (persist_exists(PERSISTENT_VERSION_KEY)) {
    version = persist_read_int(PERSISTENT_VERSION_KEY);
  }

  if (version == 0) {
    // Create new user store
    s_userData = malloc(sizeof(struct userData_v1));
    // Zero evertyhing
    memset(s_userData, 0, sizeof(struct userData_v1));
    // set some defaults
    setUserTime(30);
    setUserTimeCapacity(6000);
  } else if (version == 1) {
    // Load from memory
    int result = persist_read_data(PERSISTENT_USERDATA_KEY, s_userData, sizeof(struct userData_v1));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init_persistence readd code %i", result);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init_persistence unknown save version!");
    // todo return an error
  }

}

void destroy_persistence() { // Save
  int dataResult = persist_write_data(PERSISTENT_USERDATA_KEY, s_userData, sizeof(struct userData_v1));
  free(s_userData);
  s_userData = 0;
  int versionResult = persist_write_int(PERSISTENT_VERSION_KEY, SCHEMA_VERSION);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_persistence save code data %i schema %i", dataResult, versionResult);
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
