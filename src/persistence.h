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
  uint16_t treasuresOwned[MAX_TREASURES];
  char     uniquesOwned[BITNSLOTS(MAX_UNIQUE)];
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

uint16_t getUserOwnsUpgrades(const unsigned typeID, const unsigned resourceID);
void addUpgrade(const unsigned typeID, const unsigned resourceID);

void setUserTime(uint64_t newTime);
uint64_t getUserTime();

void setUserTotalTime(uint64_t newTime);
uint64_t getUserTotalTime();

void setUserOpt(USER_OPT opt, bool value);
bool getUserOpt(USER_OPT opt);
