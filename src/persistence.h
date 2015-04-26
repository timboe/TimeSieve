#pragma once
#include "constants.h"

// This structure defines the sum of persitifiable data
// Currently 184/256 bytes
struct userData_v1 {
  // Important time stores
  uint64_t currentTime;
  uint64_t timeCapacity;
  uint64_t totalTime;
  // Things owned
  uint16_t refineriesOwned[MAX_UPGRADES];
  uint16_t sievesOwned[MAX_UPGRADES];
  uint16_t tanksOwned[MAX_UPGRADES];
  uint16_t watchersOwned[MAX_UPGRADES];
  uint16_t treasuresOwned[MAX_TREASURES];
  char uniquesOwned[BITNSLOTS(MAX_UNIQUE)];
};

void init_persistence(); // Load
void destroy_persistence(); // Save

uint16_t getUserOwnsUpgrades(const unsigned typeID, const unsigned resourceID);

void setUserTime(uint64_t newTime);
void setUserTimeCapacity(uint64_t newTime);
uint64_t getUserTime();
uint64_t getUserTimeCapacity();