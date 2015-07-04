#include <pebble.h>
#include "persistence.h"
#include "constants.h"
#include "clock.h"
#include "timeStore.h"
#include "mainWindow.h"
#include "resources.h"

static struct userData_v1* s_userData;

/**
 * Customisable list of things to give for testing.
 */
void DEVMODE() {
  for (int i=0; i<16; ++i)  addItem(LEGENDARY_ID, i, 1);


  addItem(EPIC_ID, 3, 3);
  addItem(EPIC_ID, 2, 3);
  addItem(EPIC_ID, 1, 3);
  addItem(EPIC_ID, 0, 3);
  addItem(RARE_ID, 1, 3);
  addItem(RARE_ID, 2, 3);
  addItem(RARE_ID, 3, 3);
  addItem(RARE_ID, 0, 3);

  addItem(COMMON_ID, 3, 3);
  addItem(COMMON_ID, 0, 3);
  addItem(COMMON_ID, 1, 3);
  addItem(COMMON_ID, 2, 3);
  addItem(MAGIC_ID, 0, 3);
  addItem(MAGIC_ID, 1, 3);
  addItem(MAGIC_ID, 2, 3);
  addItem(MAGIC_ID, 3, 3);

  addUpgrade(REFINERY_ID, 12, 1);
  addUpgrade(TANK_ID, 12, 1);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "DEV MODE");
}

/**
 * Make sure all settings which need to be enacted elsewhere are
 */
void initSettings() {
  setUserSetting( SETTING_TYPE, getUserSetting(SETTING_TYPE) ); // Refrest typeface
  updateDisplayTime( getUserTime() );
}

void resetUserData() {
  // Zero evertyhing
  memset(s_userData, 0, sizeof(struct userData_v1));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "resetUserData");
  setUserOpt(OPT_ANIMATE, true);
  setUserOpt(OPT_CELSIUS, true);
  setUserSetting(SETTING_ZZZ_START, 23);
  setUserSetting(SETTING_ZZZ_END, 8);
  if (IS_DEBUG) DEVMODE();
  initSettings();
}

void init_persistence() {

  // Check if first load
  int version = 0;
  if (persist_exists(PERSISTENT_VERSION_KEY)) {
    version = persist_read_int(PERSISTENT_VERSION_KEY);
  }
  APP_LOG(APP_LOG_LEVEL_INFO, "init_persistence schema version %i", version);

  // Allocate user store memory
  s_userData = malloc(sizeof(struct userData_v1));
  if (version == 0) {
    resetUserData();
  } else if (version == 1) {
    // Load from memory
    int result = persist_read_data(PERSISTENT_USERDATA_KEY, s_userData, sizeof(struct userData_v1));
    APP_LOG(APP_LOG_LEVEL_INFO, "init_persistence read code %i", result);
    initSettings();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "init_persistence unknown save version!");
    // todo return an error
  }

}

void addUpgrade(const uint32_t typeID, const uint32_t resourceID, const int32_t n) {
  s_userData->upgradesOwned[typeID][resourceID] += n;
}

void addItemsMissed(uint32_t n) { s_userData->itemsMissed += n; }

uint32_t getItemsMissed() { return s_userData->itemsMissed; }

void addItem(const uint32_t treasureID, const uint32_t itemID, const int32_t n) {
  if (treasureID == LEGENDARY_ID) {
    BITSET(s_userData->legendaryOwned, itemID);
  } else {
    s_userData->itemsOwned[treasureID][itemID] += n;
  }
}

/**
 * Remember we cannot ever take a legendary off of the user
 **/
void removeItem(const uint32_t treasureID, const uint32_t itemID, const int32_t n) {
  addItem(treasureID, itemID, -n);
}

void destroy_persistence() { // Save
  s_userData->timeOfSave = time(NULL);
  int dataResult = persist_write_data(PERSISTENT_USERDATA_KEY, s_userData, sizeof(struct userData_v1));
  free(s_userData);
  s_userData = 0;
  int versionResult = persist_write_int(PERSISTENT_VERSION_KEY, SCHEMA_VERSION);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_persistence save code data %i schema %i", dataResult, versionResult);
}

time_t getUserTimeOfSave() {
  return s_userData->timeOfSave;
}



uint16_t getUserItems(const uint32_t treasureID, const uint32_t itemID) {
  if (treasureID == LEGENDARY_ID) {
    return (uint16_t) BITTEST(s_userData->legendaryOwned, itemID); // Going to be either 1 or 0
  } else {
    return s_userData->itemsOwned[treasureID][itemID];
  }
}

uint32_t getUserTotalItems(const uint32_t treasureID) {
  uint32_t count = 0;
  if (treasureID == LEGENDARY_ID) {
    for (uint32_t i = 0; i < MAX_UNIQUE; ++i) {
      if ( BITTEST(s_userData->legendaryOwned, i) ) ++count;
    }
  } else {
    for (uint32_t i = 0; i < MAX_TREASURES; ++i) {
      count += s_userData->itemsOwned[treasureID][i];
    }
  }
  return count;
}

uint32_t getUserGrandTotalItems() {
  uint32_t tot = 0;
  for (uint8_t cat = 0; cat < ITEM_CATEGORIES; ++cat) {
    tot += getUserTotalItems(cat);
  }
  return tot;
}

/**
 * For a given treasure type, how many different items do we have at least one of
 */
uint16_t getUserItemTypes(const uint32_t treasureID) {
  uint16_t count = 0;
  if (treasureID == LEGENDARY_ID) {
    return getUserTotalItems(treasureID);
  } else {
    for (uint32_t i = 0; i < MAX_TREASURES; ++i) {
      if (s_userData->itemsOwned[treasureID][i] > 0) ++count;
    }
  }
  return count;
}

uint16_t getUserUpgrades(const uint32_t typeID, const uint32_t resourceID) {
  return s_userData->upgradesOwned[typeID][resourceID];
}

uint32_t getUserTotalUpgrades(const uint32_t typeID) {
  uint32_t count = 0;
  for (uint32_t i = 0; i < MAX_UPGRADES; ++i) {
    count =+ getUserUpgrades(typeID, i);
  }
  return count;
}

uint32_t getUserGrandTotalUpgrades() {
  uint32_t count = 0;
  for (uint32_t i = 0; i < UPGRADE_CATEGORIES; ++i) {
    count =+ getUserTotalUpgrades(i);
  }
  return count;
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

void incrementUserSetting(USER_SETTING set) {
  setUserSetting(set, getUserSetting(set) + 1);
}

void setUserSetting(USER_SETTING set, uint8_t value) {
  if ((set == SETTING_ZZZ_END || set == SETTING_ZZZ_START) && value > 23) value = 0;
  if ((set == SETTING_LIGHT || set == SETTING_VIBE) && value >= MAX_NOTIFY_SETTINGS) value = 0;
  if (set == SETTING_TYPE && value >= FONT_MAX) value = 0;
  if (set == SETTING_COLOUR && value >= PALETTE_MAX) value = 0;
  s_userData->settings[set] = value;
  // Do we need to action on anything that has changed?
  if (set == SETTING_TYPE) {
    loadClockFont();
    switch(value) {
      case FONT_5: setClockPixelOffset(1); break;
      default: setClockPixelOffset(2);
    }
  }
}

uint8_t getUserSetting(USER_SETTING set) {
  return s_userData->settings[set];
}

void setUserOpt(USER_OPT opt, bool value) {
  if (value) {
    BITSET(s_userData->settingsBitmap, (uint16_t)opt);
  } else {
    BITCLEAR(s_userData->settingsBitmap, (uint16_t)opt);
  }
  // Do we need to action on anything that has changed?
  if (opt == OPT_SHOW_SECONDS) update_tick_handler();
  else if (opt == OPT_CELSIUS) updateWeatherBuffer();
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
