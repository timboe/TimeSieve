#include <pebble.h>
#include "persistence.h"

static struct userData_v1* s_userData;  
  
void init_persistence() {

  // Check if first load
  int version = 0;
  if (persist_exists(PERSISTENT_VERSION_KEY)) {
    version = persist_read_int(PERSISTENT_VERSION_KEY);
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init_persistence schema version %i", version);

  // Force new
  version = 0;

  // Allocate user store memory
  s_userData = malloc(sizeof(struct userData_v1));
  if (version == 0) {
    // Zero evertyhing
    memset(s_userData, 0, sizeof(struct userData_v1));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init_persistence clean");
  } else if (version == 1) {
    // Load from memory
    int result = persist_read_data(PERSISTENT_USERDATA_KEY, s_userData, sizeof(struct userData_v1));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init_persistence read code %i", result);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init_persistence unknown save version!");
    // todo return an error
  }

}

void addUpgrade(const unsigned typeID, const unsigned resourceID) {
  if (typeID == REFINERY_ID) {
    ++s_userData->refineriesOwned[resourceID];
  } else if (typeID == TANK_ID) {
    ++s_userData->tanksOwned[resourceID];
  } else if (typeID == SIEVE_ID) {
    ++s_userData->sievesOwned[resourceID];
  } else if (typeID == WATCHER_ID) {
    ++s_userData->watchersOwned[resourceID];
  }
}

void destroy_persistence() { // Save
  s_userData->timeOfSave = time(NULL);
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

uint64_t getUserTime() {
  return s_userData->currentTime;
}

void setUserTotalTime(uint64_t newTime) {
  s_userData->totalTime = newTime;
}

uint64_t getUserTotalTime() {
  return s_userData->totalTime;
}

void setUserOpt(USER_OPT opt, bool value) {
  if (value) {
    BITSET(s_userData->settingsBitmap, (uint16_t)opt);
  } else {
    BITCLEAR(s_userData->settingsBitmap, (uint16_t)opt);
  }
}

bool getUserOpt(USER_OPT opt) {
  return BITTEST(s_userData->settingsBitmap, (uint16_t)opt);
}
