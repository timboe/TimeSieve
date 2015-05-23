#pragma once
#include "constants.h"

struct catchUpSummary {
  // Important time stores
  int32_t timeDiff;
  uint64_t gainedTime;
  uint16_t missedItem;
  uint16_t gainedItems[ITEM_CATEGORIES];
};

void doCatchup();
void showSplash();
void finishSplash();
