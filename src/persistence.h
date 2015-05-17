#pragma once
#include "constants.h"

// This structure defines the sum of persitifiable data
// Currently 180/256 bytes
struct userData_v1 {
  // Important time stores
  uint64_t currentTime;
  uint64_t totalTime;
  time_t timeOfSave;
  // Things owned
  uint16_t refineriesOwned[MAX_UPGRADES];
  uint16_t tanksOwned[MAX_UPGRADES];
  uint16_t watchersOwned[MAX_UPGRADES];
  uint16_t commonOwned[MAX_TREASURES];
  uint16_t magicOwned[MAX_TREASURES];
  uint16_t rareOwned[MAX_TREASURES];
  uint16_t epicOwned[MAX_TREASURES];
  uint32_t itemsMissed;
  char     uniqueOwned[BITNSLOTS(MAX_UNIQUE)];
  char     chevoBitmap[BITNSLOTS(MAX_CHEVOS)];
  // Options
  uint8_t settings[N_USER_SETTING];
  char    settingsBitmap[BITNSLOTS(N_USER_OPT)];
  //
};

void init_persistence(); // Load
void destroy_persistence(); // Save
void resetUserData(); // Reset
void DEVMODE();

uint16_t getUserTotalUpgrades(const unsigned typeID);

uint16_t getUserItems(const unsigned treasureID, const unsigned itemID);
uint16_t getUserTotalItems(const unsigned treasureID);
uint16_t getUserGrandTotalItems();
uint16_t getUserItemTypes(const unsigned treasureID);

uint16_t getUserOwnsUpgrades(const unsigned typeID, const unsigned resourceID);

void addUpgrade(const unsigned typeID, const unsigned resourceID, const int16_t n);
void addItem(const unsigned treasureID, const unsigned itemID, const int16_t n);
void removeItem(const unsigned treasureID, const unsigned itemID, const int16_t n);

void addItemsMissed(uint32_t n);
uint32_t getItemsMissed();

time_t getUserTimeOfSave();

void setUserTime(uint64_t newTime);
uint64_t getUserTime();

void setUserTotalTime(uint64_t newTime);
uint64_t getUserTotalTime();

void setUserOpt(USER_OPT opt, bool value);
bool getUserOpt(USER_OPT opt);
void flipUserOpt(USER_OPT opt);

void incrementUserSetting(USER_SETTING set);
void setUserSetting(USER_SETTING set, uint8_t value);
uint8_t getUserSetting(USER_SETTING set);

void setUserChevo(uint8_t chevo, bool value);
bool getUserChevo(uint8_t chevo);
uint8_t getTotalChevos();
