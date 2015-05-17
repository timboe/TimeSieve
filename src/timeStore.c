#include <pebble.h>
#include <limits.h>
#include "timeStore.h"
#include "constants.h"
#include "persistence.h"
#include "items.h"

static uint64_t* s_bufferRefineryPrice;
static uint64_t* s_bufferTankPrice;
static uint64_t* s_bufferWatcherPrice;

static uint64_t* s_bufferCommonSellPrice;
static uint64_t* s_bufferMagicSellPrice;
static uint64_t* s_bufferRareSellPrice;
static uint64_t* s_bufferEpicSellPrice;

static uint64_t s_timePerMin;
static uint64_t s_displayTime;
static uint64_t s_timeCapacity;

#define INITIAL_PRICE_MODULATION 5


/**
 * While it may seem crazy that an unsigned 64 bit integer with over 631 billion year
 * capacity (while retaining second precision) could overflow - I know idle game players...
 * Note: always frame as larger+smaller. Check for modulo overflow
 * TODO: Behind the scenes, game extension via bit-shift?
 **/
uint64_t safeAdd(uint64_t a, uint64_t b) {
  return a + b;
  //if (a == ULLONG_MAX) return ULLONG_MAX;
  //uint64_t before = a;
  //a += b;
  //if (a >= before) return a;
  //return ULLONG_MAX;
}

/**
 * @see safeAdd()
 **/
uint64_t safeMultiply(uint64_t a, uint64_t b) {
  return a * b;
  //if (a == ULLONG_MAX) return ULLONG_MAX;
  //uint64_t before = a;
  //a *= b;
  //if (a >= before) return a;
  //return ULLONG_MAX;
}

// Perform fixed point increase in price by floor of 7/6.
uint64_t getPriceOfNext(uint64_t priceOfCurrent) {
  priceOfCurrent /= INCREASE_DIVIDE;
  return safeMultiply(priceOfCurrent, INCREASE_MULTIPLY);
}

uint64_t getItemBasePrice(const unsigned treasureID, const unsigned itemID) {
  if (treasureID == COMMON_ID) {
    return SELL_PRICE_COMMON[itemID];
  } else if (treasureID == MAGIC_ID) {
    return SELL_PRICE_MAGIC[itemID];
  } else if (treasureID == RARE_ID) {
    return SELL_PRICE_RARE[itemID];
  } else if (treasureID == EPIC_ID) {
    return SELL_PRICE_EPIC[itemID];
  }
  return 0;
}

/**
 * Every 1m, we modulate the prices, very basic
 */
void modulateSellPrices() {
  for (unsigned item = 0; item < MAX_TREASURES; ++item ) {
    s_bufferCommonSellPrice[item] += ( (SELL_PRICE_COMMON[item]/(uint64_t)100) * (-5+(rand()%11)) );
    s_bufferMagicSellPrice[item]  += ( (SELL_PRICE_MAGIC[item]/(uint64_t)100)  * (-5+(rand()%11)) );
    s_bufferRareSellPrice[item]   += ( (SELL_PRICE_RARE[item]/(uint64_t)100)   * (-5+(rand()%11)) );
    s_bufferEpicSellPrice[item]   += ( (SELL_PRICE_EPIC[item]/(uint64_t)100)   * (-5+(rand()%11)) );

    // Prevent from dropping *too* low
    if (s_bufferCommonSellPrice[item] < SELL_PRICE_COMMON[item]/(uint64_t)4) {
      s_bufferCommonSellPrice[item] += ( (SELL_PRICE_COMMON[item]/(uint64_t)100) * (5+(rand()%6)) );
    }
    if (s_bufferMagicSellPrice[item] < SELL_PRICE_MAGIC[item]/(uint64_t)4) {
      s_bufferMagicSellPrice[item] += ( (SELL_PRICE_MAGIC[item]/(uint64_t)100) * (5+(rand()%6)) );
    }
    if (s_bufferRareSellPrice[item] < SELL_PRICE_RARE[item]/(uint64_t)4) {
      s_bufferRareSellPrice[item] += ( (SELL_PRICE_RARE[item]/(uint64_t)100) * (5+(rand()%6)) );
    }
    if (s_bufferEpicSellPrice[item] < SELL_PRICE_EPIC[item]/(uint64_t)4) {
      s_bufferEpicSellPrice[item] += ( (SELL_PRICE_EPIC[item]/(uint64_t)100) * (5+(rand()%6)) );
    }
  }
}

/**
 * Load the const default prices into a buffer so we can play with them
 */
void initiateSellPrices() {

  s_bufferCommonSellPrice = (uint64_t*) malloc( MAX_TREASURES*sizeof(uint64_t) );
  s_bufferMagicSellPrice  = (uint64_t*) malloc( MAX_TREASURES*sizeof(uint64_t) );
  s_bufferRareSellPrice   = (uint64_t*) malloc( MAX_TREASURES*sizeof(uint64_t) );
  s_bufferEpicSellPrice   = (uint64_t*) malloc( MAX_TREASURES*sizeof(uint64_t) );

  // Load defaults
  for (unsigned item = 0; item < MAX_TREASURES; ++item ) {
    s_bufferCommonSellPrice[item] = getItemBasePrice(COMMON_ID, item);
    s_bufferMagicSellPrice[item]  = getItemBasePrice(MAGIC_ID, item);
    s_bufferRareSellPrice[item]   = getItemBasePrice(RARE_ID, item);
    s_bufferEpicSellPrice[item]   = getItemBasePrice(EPIC_ID, item);
  }

  // Do initial permuting
  for (uint16_t n = 0; n < INITIAL_PRICE_MODULATION; ++n) {
    modulateSellPrices();
  }
}

void init_timeStore() {

  s_bufferRefineryPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferTankPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferWatcherPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );

  // Populate the buffer. This could take a little time, do it at the start
  APP_LOG(APP_LOG_LEVEL_DEBUG, "SBufr");
  for (unsigned upgrade = 0; upgrade < MAX_UPGRADES; ++upgrade ) {
    uint16_t nOwned = getUserOwnsUpgrades(REFINERY_ID, upgrade);
    uint64_t currentPrice = INITIAL_PRICE_REFINERY[upgrade];
    for (unsigned i = 0; i < nOwned; ++i) currentPrice = getPriceOfNext(currentPrice);
    s_bufferRefineryPrice[upgrade] = currentPrice;
    //
    nOwned = getUserOwnsUpgrades(TANK_ID, upgrade);
    currentPrice = INITIAL_PRICE_TANK[upgrade];
    for (unsigned i = 0; i < nOwned; ++i) currentPrice = getPriceOfNext(currentPrice);
    s_bufferTankPrice[upgrade] = currentPrice;
    //
    // Note watchers increase a lot more, a factor 2 here
    nOwned = getUserOwnsUpgrades(WATCHER_ID, upgrade);
    currentPrice = INITIAL_PRICE_WATCHER[upgrade];
    for (unsigned i = 0; i < nOwned; ++i) currentPrice = safeMultiply(currentPrice, INCREASE_WATCHER);
    s_bufferWatcherPrice[upgrade] = currentPrice;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "FBufr");

  updateTimePerMin();
  updateTankCapacity();
  initiateSellPrices();
  updateProbabilities();
}

void destroy_timeStore() {
  free( s_bufferRefineryPrice );
  free( s_bufferTankPrice );
  free( s_bufferWatcherPrice );

  free( s_bufferCommonSellPrice );
  free( s_bufferMagicSellPrice );
  free( s_bufferRareSellPrice );
  free( s_bufferEpicSellPrice );
}

void giveCatchupItems(TimeUnits units, uint32_t n) {
  for (uint32_t i = 0; i < n; ++i) {
    uint8_t treasureID = getItemRarity(units);
    uint8_t itemID = 0;
    genRandomItem(&treasureID, &itemID); // Both are modified (rarity may need to be downgraded)
    addItem(treasureID, itemID, 1);
  }
}

void doCatchup() {
  if (getUserTimeOfSave() == 0) return;
  int32_t timeDiff = time(NULL) - getUserTimeOfSave();
  APP_LOG(APP_LOG_LEVEL_INFO, "Catchup BGN %i seconds", (int)timeDiff);
  if (timeDiff < 0) return;

  // Give liquid time. Won't give more than can fit
  uint32_t nMin = timeDiff / SEC_IN_MIN;
  addTime( safeMultiply( getTimePerMin(), nMin) );

  // If no chance of autocollect then end here!
  if (getAutoCollectChance() == 0) return;

  // Items
  uint32_t nYear = timeDiff / SEC_IN_YEAR;
  uint32_t nMonth = (timeDiff / (SEC_IN_DAY*31)) - nYear;
  uint32_t nDay = (timeDiff / SEC_IN_DAY) - nMonth - nYear;
  uint32_t nHour = (timeDiff / SEC_IN_HOUR) - nDay - nMonth - nYear;
  nMin = nMin - nHour - nDay - nMonth - nYear;
  uint32_t itemsFound = 0;
  // For year, month and day we do it proper, i.e. we check each boundary in turn
  for (uint8_t N = 0; N < 3; ++N) {
    uint32_t tries = nYear;
    TimeUnits unit = YEAR_UNIT;
    if (N == 1) {
      tries = nMonth;
      unit = MONTH_UNIT;
    } else if (N == 2) {
      tries = nDay;
      unit = DAY_UNIT;
    }
    for (uint32_t t = 0; t < tries; ++t) {
      if (getItemAutoCollect() == false) continue;
      if (checkForItem(unit) == false) continue;
      giveCatchupItems(unit, 1);
      ++itemsFound;
    }
  }
  // For hour and min, we cheat, and do it probabailisticaly.
  // This rounds down to the nearest items boo-hoo to the user
  // Expected N items
  uint32_t dayItems = (nDay * getItemAppearChance(DAY_UNIT)) / SCALE_FACTOR;
  // Now take into account the auto-collect chance
  dayItems = (dayItems * getAutoCollectChance()) / SCALE_FACTOR;
  // Finally give the items
  giveCatchupItems(DAY_UNIT, dayItems);
  // And do the same for mins
  uint32_t minItems = (nMin * getItemAppearChance(MINUTE_UNIT)) / SCALE_FACTOR;
  minItems = (minItems * getAutoCollectChance()) / SCALE_FACTOR;
  giveCatchupItems(MINUTE_UNIT, minItems);
  // Done
  itemsFound += dayItems + minItems;
  APP_LOG(APP_LOG_LEVEL_INFO, "Catchup END %i items", (int)itemsFound);

}


uint64_t getPriceOfUpgrade(const unsigned typeID, const unsigned resourceID) {
  uint64_t currentPrice = 0;
  if (typeID == REFINERY_ID) {
    currentPrice = s_bufferRefineryPrice[resourceID];
  } else if (typeID == TANK_ID) {
    currentPrice = s_bufferTankPrice[resourceID];
  } else if (typeID == WATCHER_ID) {
    currentPrice = s_bufferWatcherPrice[resourceID];
  }

  return getPriceOfNext(currentPrice);
}

uint64_t getCurrentSellPrice(const unsigned treasureID, const unsigned itemID) {
  if (treasureID == COMMON_ID)     return s_bufferCommonSellPrice[itemID];
  else if (treasureID == MAGIC_ID) return s_bufferMagicSellPrice[itemID];
  else if (treasureID == RARE_ID)  return s_bufferRareSellPrice[itemID];
  else if (treasureID == EPIC_ID)  return s_bufferEpicSellPrice[itemID];
  return 0;
}

void currentSellPricePercentage(char* buffer, const size_t buffer_size, unsigned* value, const unsigned treasureID, const unsigned itemID) {
  uint64_t basePrice = getItemBasePrice(treasureID, itemID);
  uint64_t currentPrice = getCurrentSellPrice(treasureID, itemID);
  percentageToString(currentPrice, basePrice, buffer, buffer_size, value, false);
}

/**
 * Get the face value for all items of a given category. Won't overflow
 **/
uint64_t currentCategorySellPrice(const unsigned treasureID) {
  uint64_t sellPrice = 0;
  for (uint8_t i = 0; i < MAX_TREASURES; ++i) sellPrice += getCurrentSellPrice(treasureID, i);
  return sellPrice;
}

uint64_t currentTotalSellPrice() {
  uint64_t tot = 0;
  for (uint8_t c = 0; c < SELLABLE_CATEGORIES; ++c) {
    tot = safeAdd(tot,currentCategorySellPrice(c));
  }
  return tot;
}

uint64_t getTimePerMin() {
  return s_timePerMin;
}

/**
 * Redo the calculation about how much time we should be making every min
 * and take into acount all bonuses. Won't overflow
 **/
void updateTimePerMin() {
  s_timePerMin = 60; // This is the base level
  for (unsigned upgrade = 0; upgrade < MAX_UPGRADES; ++upgrade ) {
    s_timePerMin += getUserOwnsUpgrades(REFINERY_ID, upgrade) * REWARD_REFINERY[upgrade];
  }
}

uint64_t getTankCapacity() {
  return s_timeCapacity;
}

/**
 * Total capacity, *COULD OVERFLOW*, devote extra preventative CPU againsts this
 **/
void updateTankCapacity() {
  s_timeCapacity = SEC_IN_HOUR; // Base level
  for (unsigned upgrade = 0; upgrade < MAX_UPGRADES; ++upgrade ) {
    s_timeCapacity = safeAdd( s_timeCapacity, safeMultiply( REWARD_TANK[upgrade], getUserOwnsUpgrades(TANK_ID, upgrade)) );
  }
}

uint64_t getDisplayTime() {
  return s_displayTime;
}

void updateDisplayTime(uint64_t t) {
  s_displayTime = t;
}

/**
 * Update the user's time while respecting their tank size
 **/
void addTime(uint64_t toAdd) {
  if (safeAdd(getUserTime(), toAdd) > getTankCapacity()) {
    toAdd = getTankCapacity() - getUserTime();
  }
  setUserTime( safeAdd( getUserTime(), toAdd) );
  setUserTotalTime( safeAdd( getUserTotalTime(), toAdd ));
}

/**
 * Remove time from user, prevent underflow
 **/
void removeTime(uint64_t toSubtract) {
  if (toSubtract > getUserTime()) toSubtract = getUserTime();
  setUserTime( getUserTime() - toSubtract );
}

/**
 * Check that the desired item can be afforded, and buy it if so
 */
bool doPurchase(const unsigned typeID, const unsigned resourceID) {
  uint64_t cost = getPriceOfUpgrade(typeID, resourceID);
  if (cost > getUserTime()) return false;
  removeTime( cost ); // Debit the user
  addUpgrade(typeID, resourceID, 1); // Give them their upgrade
  // Update the price in the buffer so the next one becomes more expensive
  // And recalculate factors
  if (typeID == REFINERY_ID) {
    s_bufferRefineryPrice[resourceID] = cost;
    updateTimePerMin();
  } else if (typeID == TANK_ID) {
    s_bufferTankPrice[resourceID] = cost;
    updateTankCapacity();
  } else if (typeID == WATCHER_ID) {
    s_bufferWatcherPrice[resourceID] = cost;
    updateProbabilities();
  }
  return true;
}


void timeToString(uint64_t time, char* buffer, size_t buffer_size, bool brief) {

  int eons = time / SEC_IN_EON;
  int eras = (time % SEC_IN_EON) / SEC_IN_ERA;

  // 0_o
  if (brief && time == ULLONG_MAX) {
    snprintf(buffer, buffer_size, "%iEon: MAX!!!", eons);
    return;
  }

  if (brief && eons) {
    snprintf(buffer, buffer_size, "%iEon %iEra", eons, eras);
    return;
  }

  int epochs = (time % SEC_IN_ERA) / SEC_IN_EPOCH;

  if (brief && eras) {
    snprintf(buffer, buffer_size, "%iEra %iEpoch", eras, epochs);
    return;
  }

  int ages = (time % SEC_IN_EPOCH) / SEC_IN_AGE;

  if (brief && epochs) {
    snprintf(buffer, buffer_size, "%iEpoch %iAge", epochs, ages);
    return;
  }

  int mills  =  (time % SEC_IN_AGE) / SEC_IN_MILLENIUM;

  if (brief && ages) {
    snprintf(buffer, buffer_size, "%iAge %iM", ages, mills);
    return;
  }

  int years = (time % SEC_IN_MILLENIUM) / SEC_IN_YEAR;

  if (brief && mills) {
    snprintf(buffer, buffer_size, "%iM %iy", mills, years);
    return;
  }

  int days  = (time % SEC_IN_YEAR) / SEC_IN_DAY;
  int hours = (time % SEC_IN_DAY) / SEC_IN_HOUR;

  if (brief && years) {
    snprintf(buffer, buffer_size, "%iy %id %ih", years, days, hours);
    return;
  }

  int mins = (time % SEC_IN_HOUR) / SEC_IN_MIN;

  if (brief && days) {
    snprintf(buffer, buffer_size, "%id %ih %im", days, hours, mins);
    return;
  }

  int secs = time % SEC_IN_MIN;

  if (brief && hours) {
    snprintf(buffer, buffer_size, "%ih %im %is", hours, mins, secs);
    return;
  } else if (brief && mins) {
    snprintf(buffer, buffer_size, "%im %is", mins, secs);
    return;
  } else if (brief) {
    snprintf(buffer, buffer_size, "%is", secs);
    return;
  }

  // Full
  if (time == ULLONG_MAX) snprintf(buffer, buffer_size, "MAX!:%iEon %iEra %iEpoch %iAge %iM %iy %id %ih %im %is", eons, eras, epochs, ages, mills, years, days, hours, mins, secs);
  else if (eons) snprintf(buffer, buffer_size, "%iEon %iEra %iEpoch %iAge %iM %iy %id %ih %im %is", eons, eras, epochs, ages, mills, years, days, hours, mins, secs);
  else if (eras) snprintf(buffer, buffer_size, "%iEra %iEpoch %iAge %iM %iy %id %ih %im %is", eras, epochs, ages, mills, years, days, hours, mins, secs);
  else if (epochs) snprintf(buffer, buffer_size, "%iEpoch %iAge %iM %iy %id %ih %im %is", epochs, ages, mills, years, days, hours, mins, secs);
  else if (ages) snprintf(buffer, buffer_size, "%iAge %iM %iy %id %ih %im %is", ages, mills, years, days, hours, mins, secs);
  else if (mills) snprintf(buffer, buffer_size, "%iM %iy %id %ih %im %is", mills, years, days, hours, mins, secs);
  else if (years) snprintf(buffer, buffer_size, "%iy %id %ih %im %is", years, days, hours, mins, secs);
  else if (days) snprintf(buffer, buffer_size, "%id %ih %im %is", days, hours, mins, secs);
  else if (hours) snprintf(buffer, buffer_size, "%ih %im %is", hours, mins, secs);
  else if (mins) snprintf(buffer, buffer_size, "%im %is", mins, secs);
  else snprintf(buffer, buffer_size, "%is", secs);
  return;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Did time %iy %id %ih %im %is", _years, days, hours, mins, secs);
}

void percentageToString(uint64_t amount, uint64_t total, char* buffer, size_t bufferSize, unsigned* value, bool extraDigits) {
  *value = (amount * 100) / total;
  unsigned remain = (amount * 100) % total;
  // We only want the two most signnificant figs
  if (extraDigits) snprintf(buffer, 6*sizeof(char), "%i.%i%%", *value, remain);
  else snprintf(buffer, bufferSize, "%i", *value);
  return;
}
