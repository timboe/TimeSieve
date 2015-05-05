#include <pebble.h>
#include "persistence.h"
#include "constants.h"
#include "clock.h"
#include "timeStore.h"
#include "mainWindow.h"

static struct userData_v1* s_userData;  

/**
 * Customisable list of things to give for testing.
 */
void DEVMODE() {
  addItem(LEGENDARY_ID, 2);
  addItem(LEGENDARY_ID, 3);
  addItem(EPIC_ID, 3);
  addItem(EPIC_ID, 2);
  addItem(EPIC_ID, 1);
  addItem(EPIC_ID, 0);
  addItem(RARE_ID, 1);
  addItem(RARE_ID, 2);
  addItem(RARE_ID, 3);
  addItem(RARE_ID, 0);

  addItem(COMMON_ID, 3);
  addItem(COMMON_ID, 0);
  addItem(COMMON_ID, 1);
  addItem(COMMON_ID, 2);
  addItem(MAGIC_ID, 0);
  addItem(MAGIC_ID, 1);
  addItem(MAGIC_ID, 2);
  addItem(MAGIC_ID, 3);
  //addItem(RARE_ID, 0);
  addTime((uint64_t)300);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "DEV MODE");
  updateDisplayTime( getUserTime() );
}

/**
 * Make sure all settings which need to be enacted elsewhere are
 */
void initSettings() {
  setUserTypeSetting( s_userData->typeSetting );
  updateDisplayTime( getUserTime() );
}

void resetUserData() {
  // Zero evertyhing
  memset(s_userData, 0, sizeof(struct userData_v1));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "resetUserData");
  setUserOpt(OPT_ANIMATE, true);
  initSettings();
}
  
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
    resetUserData();
  } else if (version == 1) {
    // Load from memory
    int result = persist_read_data(PERSISTENT_USERDATA_KEY, s_userData, sizeof(struct userData_v1));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init_persistence read code %i", result);
    initSettings();
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

void addItem(const unsigned treasureID, const unsigned itemID) {
  if (treasureID == COMMON_ID) {
    ++s_userData->commonOwned[itemID];
  } else if (treasureID == MAGIC_ID) {
    ++s_userData->magicOwned[itemID];
  } else if (treasureID == RARE_ID) {
    ++s_userData->rareOwned[itemID];
  } else if (treasureID == EPIC_ID) {
    ++s_userData->epicOwned[itemID];
  } else if (treasureID == LEGENDARY_ID) {
    BITSET(s_userData->uniqueOwned, itemID);
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

uint16_t getUserTotalUpgrades(const unsigned typeID) {
  uint16_t count = 0;
  for (unsigned i = 0; i < MAX_UPGRADES; ++i) {
    if (typeID == REFINERY_ID) {
      count += s_userData->refineriesOwned[i];
    } else if (typeID == TANK_ID) {
      count += s_userData->tanksOwned[i];
    } else if (typeID == SIEVE_ID) {
      count += s_userData->sievesOwned[i];
    } else if (typeID == WATCHER_ID) {
      count += s_userData->watchersOwned[i];
    }
  }
  return count;
}

uint16_t getUserItems(const unsigned treasureID, const unsigned itemID) {
  if (treasureID == LEGENDARY_ID) {
    return (uint16_t) BITTEST(s_userData->uniqueOwned, itemID); // Going to be either 1 or 0
  } else {
    if (treasureID == COMMON_ID) {
      return s_userData->commonOwned[itemID];
    } else if (treasureID == MAGIC_ID) {
      return s_userData->magicOwned[itemID];
    } else if (treasureID == RARE_ID) {
      return s_userData->rareOwned[itemID];
    } else if (treasureID == EPIC_ID) {
      return s_userData->epicOwned[itemID];
    }
  }
  return 0;
}

uint16_t getUserTotalItems(const unsigned treasureID) {
  uint16_t count = 0;
  if (treasureID == LEGENDARY_ID) {
    for (unsigned i = 0; i < MAX_UNIQUE; ++i) {
      if ( BITTEST(s_userData->uniqueOwned, i) ) ++count;
    }
  } else {
    for (unsigned i = 0; i < MAX_TREASURES; ++i) {
      if (treasureID == COMMON_ID) {
        count += s_userData->commonOwned[i];
      } else if (treasureID == MAGIC_ID) {
        count += s_userData->magicOwned[i];
      } else if (treasureID == RARE_ID) {
        count += s_userData->rareOwned[i];
      } else if (treasureID == EPIC_ID) {
        count += s_userData->epicOwned[i];
      }
    }
  }
  return count;
}

/**
 * For a given treasure type, how many different items do we have at least one of
 */
uint16_t getUserItemTypes(const unsigned treasureID) {
  uint16_t count = 0;
  if (treasureID == LEGENDARY_ID) {
    return getUserTotalItems(treasureID);
  } else {
    for (unsigned i = 0; i < MAX_TREASURES; ++i) {
      if (treasureID == COMMON_ID && s_userData->commonOwned[i] > 0) {
        ++count;
      } else if (treasureID == MAGIC_ID && s_userData->magicOwned[i] > 0) {
        ++count;
      } else if (treasureID == RARE_ID && s_userData->rareOwned[i] > 0) {
        ++count;
      } else if (treasureID == EPIC_ID && s_userData->epicOwned[i] > 0) {
        ++count;
      }
    }
  }
  return count;
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

void incrementUserColorTheme() {
  if (++(s_userData->colourTheme) == PALETTE_MAX) s_userData->colourTheme = 0;
}

uint8_t getUserColorTheme() {
  return s_userData->colourTheme; 
}

void setUserTypeSetting(uint8_t value) {
  if (value >= FONT_MAX) value = 0;
  s_userData->typeSetting = value;
  switch(s_userData->typeSetting) {
    case FONT_5: setClockPixelOffset(1); break;
    default: setClockPixelOffset(2);
  }
}

void incrementUserTypeSetting() {
  setUserTypeSetting( s_userData->typeSetting + 1 );
}

uint8_t getUserTypeSetting() {
  return s_userData->typeSetting; 
}

void incrementUserLightSetting() {
  setUserLightSetting( s_userData->lightSetting + 1 );
}

void setUserLightSetting(uint8_t value) {
  if (value >= MAX_NOTIFY_SETTINGS) value = 0;
  s_userData->lightSetting = value;
}

uint8_t getUserLightSetting() {
  return s_userData->lightSetting; 
}

void incrementUserVibrateSetting() {
  setUserVibrateSetting( s_userData->vibrateSetting + 1 );
}

void setUserVibrateSetting(uint8_t value) {
  if (value >= MAX_NOTIFY_SETTINGS) value = 0;
  s_userData->vibrateSetting = value;
}

uint8_t getUserVibrateSetting() {
  return s_userData->vibrateSetting; 
}

void incrementUserZzzStartSetting() {
  setUserZzzStartSetting( s_userData->zzzStartSetting + 1 );
}

void setUserZzzStartSetting(uint8_t value) {
  if (value >= 24) value = 0;
  s_userData->zzzStartSetting = value;
}

uint8_t getUserZzzStartSetting() {
  return s_userData->zzzStartSetting;
}

void incrementUserZzzEndSetting() {
  setUserZzzEndSetting( s_userData->zzzEndSetting + 1 );
}

void setUserZzzEndSetting(uint8_t value) {
  if (value >= 24) value = 0;
  s_userData->zzzEndSetting = value;
}

uint8_t getUserZzzEndSetting() {
  return s_userData->zzzEndSetting;
}

void setUserOpt(USER_OPT opt, bool value) {
  if (value) {
    BITSET(s_userData->settingsBitmap, (uint16_t)opt);
  } else {
    BITCLEAR(s_userData->settingsBitmap, (uint16_t)opt);
  }

  // Do we need to action on anything that has changed?
  if (opt == OPT_SHOW_SECONDS) update_tick_handler();
}

void flipUserOpt(USER_OPT opt) {
  setUserOpt(opt, !getUserOpt(opt));
}

bool getUserOpt(USER_OPT opt) {
  return BITTEST(s_userData->settingsBitmap, (uint16_t)opt);
}

void setUserChevo(uint8_t chevo, bool value) {
  if (value) {
    BITSET(s_userData->chevoBitmap, chevo);
  } else {
    BITCLEAR(s_userData->chevoBitmap, chevo);
  }
}

bool getUserChevo(uint8_t chevo) {
  return BITTEST(s_userData->chevoBitmap, chevo);
}

uint8_t getTotalChevos() {
  uint8_t count = 0;
  for (unsigned i = 0; i < MAX_CHEVOS; ++i) {
    if ( BITTEST(s_userData->chevoBitmap, i) ) ++count;
  }
  return count;
}