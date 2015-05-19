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
  uint16_t upgradesOwned[UPGRADE_CATEGORIES][MAX_UPGRADES];
  uint16_t itemsOwned[SELLABLE_CATEGORIES][MAX_TREASURES];
  uint32_t itemsMissed;
  char     legendaryOwned[BITNSLOTS(MAX_UNIQUE)];
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


uint16_t getUserItems(const uint32_t treasureID, const uint32_t itemID);
uint16_t getUserTotalItems(const uint32_t treasureID);
uint16_t getUserGrandTotalItems();
uint16_t getUserItemTypes(const uint32_t treasureID);

uint16_t getUserUpgrades(const uint32_t typeID, const uint32_t resourceID);
uint16_t getUserTotalUpgrades(const uint32_t typeID);

void addUpgrade(const uint32_t typeID, const uint32_t resourceID, const int32_t n);
void addItem(const uint32_t treasureID, const uint32_t itemID, const int32_t n);
void removeItem(const uint32_t treasureID, const uint32_t itemID, const int32_t n);

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
