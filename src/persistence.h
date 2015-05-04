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
  uint16_t sievesOwned[MAX_UPGRADES];
  uint16_t tanksOwned[MAX_UPGRADES];
  uint16_t watchersOwned[MAX_UPGRADES];
  uint16_t commonOwned[MAX_TREASURES];
  uint16_t magicOwned[MAX_TREASURES];
  uint16_t rareOwned[MAX_TREASURES];
  uint16_t epicOwned[MAX_TREASURES];
  char     uniqueOwned[BITNSLOTS(MAX_UNIQUE)];
  char     chevoBitmap[BITNSLOTS(MAX_CHEVOS)];
  // Options
  uint8_t colourTheme;
  uint8_t typeSetting;
  uint8_t lightSetting;
  uint8_t vibrateSetting;
  // 
  uint8_t zzzStartSetting;
  uint8_t zzzEndSetting;
  uint8_t aSetting;
  uint8_t bSetting;
  //
  uint8_t cSetting;
  uint8_t dSetting;
  char    settingsBitmap[BITNSLOTS(N_USER_OPT)];
  //
};

void init_persistence(); // Load
void destroy_persistence(); // Save
void resetUserData(); // Reset

uint16_t getUserTotalUpgrades(const unsigned typeID);
uint16_t getUserTotalItems(const unsigned treasureID);
uint16_t getUserItemTypes(const unsigned treasureID);

uint16_t getUserOwnsUpgrades(const unsigned typeID, const unsigned resourceID);
void addUpgrade(const unsigned typeID, const unsigned resourceID);
void addItem(const unsigned treasureID, const unsigned itemID);

void setUserTime(uint64_t newTime);
uint64_t getUserTime();

void setUserTotalTime(uint64_t newTime);
uint64_t getUserTotalTime();

void setUserOpt(USER_OPT opt, bool value);
bool getUserOpt(USER_OPT opt);
void flipUserOpt(USER_OPT opt);

void incrementUserColorTheme();
uint8_t getUserColorTheme();

void incrementUserTypeSetting();
void setUserTypeSetting(uint8_t value);
uint8_t getUserTypeSetting();

void incrementUserLightSetting();
void setUserLightSetting(uint8_t value);
uint8_t getUserLightSetting();

void incrementUserVibrateSetting();
void setUserVibrateSetting(uint8_t value);
uint8_t getUserVibrateSetting();

void incrementUserZzzStartSetting();
void setUserZzzStartSetting(uint8_t value);
uint8_t getUserZzzStartSetting();

void incrementUserZzzEndSetting();
void setUserZzzEndSetting(uint8_t value);
uint8_t getUserZzzEndSetting();

void setUserChevo(uint8_t chevo, bool value);
bool getUserChevo(uint8_t chevo);
uint8_t getTotalChevos();