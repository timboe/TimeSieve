#include <pebble.h>
#include <limits.h>
#include "timeStore.h"
#include "constants.h"
#include "persistence.h"
#include "items.h"

static uint64_t* s_bufferUpgradePrice;
static uint64_t* s_bufferItemSellPrice;

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
// Watchers have their own upgrade path, nominally x10
uint64_t getPriceOfNext(uint64_t priceOfCurrent, uint32_t typeID) {
  if (typeID == WATCHER_ID) return safeMultiply(priceOfCurrent, INCREASE_WATCHER);
  priceOfCurrent /= INCREASE_DIVIDE;
  return safeMultiply(priceOfCurrent, INCREASE_MULTIPLY);
}

uint64_t getItemBasePrice(const uint32_t treasureID, const uint32_t itemID) {
  uint64_t price = SELL_PRICE[treasureID][itemID];
  // LEGENDARY BONUS - items base price up by 5%
  if ( getUserItems(LEGENDARY_ID, ITEMSELL_5PERC) == 1 ) {
    price = (price * 21) / 20;
  }
  return price;
}

/**
 * Every 1m, we modulate the prices, very basic
 */
void modulateSellPrices() {
  int8_t lower = -5; //[-5 ... 5]
  int8_t range = 11;

  // LEGENDARY BONUS - markets move more in your favour
  if ( getUserItems(LEGENDARY_ID, MARKET) == 1 ) {
    lower = -4; //[-4 ... 5]
    range = 10;
  }

  for (uint32_t item = 0; item < MAX_TREASURES; ++item ) {
    for (uint32_t cat = 0; cat < SELLABLE_CATEGORIES; ++cat) {
      s_bufferItemSellPrice[(cat*MAX_TREASURES)+item] += ( (getItemBasePrice(cat,item)/(uint64_t)100) * (lower+(rand()%range)) );
      // Prevent from dropping *too* low
      if (s_bufferItemSellPrice[(cat*MAX_TREASURES)+item] < getItemBasePrice(cat,item)/(uint64_t)4) {
        s_bufferItemSellPrice[(cat*MAX_TREASURES)+item] += ( (getItemBasePrice(cat,item)/(uint64_t)100) * (5+(rand()%6)) );
      }
    }
  }
}

/**
 * Load the const default prices into a buffer so we can play with them
 */
void initiateSellPrices() {
  s_bufferItemSellPrice = (uint64_t*) malloc( SELLABLE_CATEGORIES*MAX_TREASURES*sizeof(uint64_t) );

  // Load defaults
  for (uint32_t item = 0; item < MAX_TREASURES; ++item ) {
    for (uint32_t cat = 0; cat < SELLABLE_CATEGORIES; ++cat) {
      s_bufferItemSellPrice[(cat*MAX_TREASURES)+item] = getItemBasePrice(cat, item);
    }
  }

  // Do initial permuting
  for (uint32_t n = 0; n < INITIAL_PRICE_MODULATION; ++n) {
    modulateSellPrices();
  }
}

void init_timeStore() {

  s_bufferUpgradePrice = (uint64_t*) malloc( UPGRADE_CATEGORIES*MAX_UPGRADES*sizeof(uint64_t) );

  // Populate the buffer. This could take a little time, do it at the start
  APP_LOG(APP_LOG_LEVEL_DEBUG, "SBufr");
  for (uint32_t upgrade = 0; upgrade < MAX_UPGRADES; ++upgrade ) {
    for (uint32_t cat = 0; cat < UPGRADE_CATEGORIES; ++cat) {
      uint16_t nOwned = getUserUpgrades(cat, upgrade);
      uint64_t currentPrice = UPGRADE_PRICE[cat][upgrade];
      //APP_LOG(APP_LOG_LEVEL_INFO,"%i,%i price is %u", (int)cat, (int) upgrade, (unsigned int)currentPrice);
      for (uint32_t i = 0; i < nOwned; ++i) currentPrice = getPriceOfNext(currentPrice, cat);
      s_bufferUpgradePrice[(cat*MAX_UPGRADES)+upgrade] = currentPrice;
    }
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "FBufr");

  updateTimePerMin();
  updateTankCapacity();
  initiateSellPrices();
  updateProbabilities();
}

void destroy_timeStore() {
  free( s_bufferItemSellPrice );
  free( s_bufferUpgradePrice );
}


bool getIfAtMaxLevel(uint32_t typeID, uint32_t resourceID) {
  if (typeID != WATCHER_ID) return false;
  const int owned = getUserUpgrades(typeID, resourceID);
  switch (resourceID) {
    case WATCHER_TECH:
      if (owned == TECH_WEATHER) return true;
      break;
    case WATCHER_LIGHT: case WATCHER_VIBE:
      if (owned == NOTIFY_LEGENDARY) return true;
      break;
    case WATCHER_FONT:
      if (owned == FONT_5) return true;
      break;
    case WATCHER_COLOUR:
      if (owned == PALETTE_RED) return true;
      break;
  }
  return false;
}

uint64_t getPriceOfUpgrade(const uint32_t typeID, const uint32_t resourceID) {
  uint64_t price = getPriceOfNext(s_bufferUpgradePrice[(typeID*MAX_UPGRADES)+resourceID], typeID);

  //APP_LOG(APP_LOG_LEVEL_INFO,"%i,%i PRICE OF NEXT %u -> %u", (int)typeID, (int) resourceID, (unsigned int)s_bufferUpgradePrice[(typeID*MAX_UPGRADES)+resourceID], (unsigned int) price);

  // LEGENDARY BONUS - 2% discount on refineries
  if ( getUserItems(LEGENDARY_ID, REFINERY_2PERC) == 1 ) {
    price = safeMultiply(price, 49) / 50;
  }
  // LEGENDARY BONUS - 4% discount on tanks
  if ( getUserItems(LEGENDARY_ID, TANKUP_4PERC) == 1 ) {
    price = safeMultiply(price, 24) / 25;
  }
  // LEGENDARY BONUS - 4% discount on employees
  if ( getUserItems(LEGENDARY_ID, EMPLOYEE_4PERC) == 1 ) {
    price = safeMultiply(price, 24) / 25;
  }
  return price;
}

uint64_t getCurrentSellPrice(const uint32_t treasureID, const uint32_t itemID) {
  return s_bufferItemSellPrice[(treasureID*MAX_TREASURES)+itemID];
}

void currentSellPricePercentage(char* buffer, const size_t buffer_size, unsigned* value, const uint32_t treasureID, const uint32_t itemID) {
  uint64_t basePrice = getItemBasePrice(treasureID, itemID);
  uint64_t currentPrice = getCurrentSellPrice(treasureID, itemID);
  percentageToString(currentPrice, basePrice, buffer, buffer_size, value, false);
}

/**
 * Get the face value for all items of a given category. Won't overflow //TODO this is wrong! want the sum of the sell prices of the owned items
 **/
uint64_t currentCategorySellPrice(const uint32_t treasureID) {
  uint64_t sellPrice = 0;
  for (uint8_t i = 0; i < MAX_TREASURES; ++i) {
    sellPrice += s_bufferItemSellPrice[(treasureID*MAX_TREASURES)+i];
  }
  return sellPrice;
}

uint64_t currentTotalSellPrice() {
  uint64_t total = 0;
  for (uint32_t c = 0; c < SELLABLE_CATEGORIES; ++c) {
    total = safeAdd(total, currentCategorySellPrice(c));
  }
  return total;
}

uint64_t getTimePerMin() {
  uint16_t bonus = 100;
  // LEGENDARY BONUS - 1% bonus time per 100 objects owned. An interesting one
  if ( getUserItems(LEGENDARY_ID, TPSBONUS_100ITEM) == 1 ) {
    bonus += (getUserGrandTotalItems() / 100);
  }
  // ACHIEVEMENT BONUS - 1% bonus time per achievement
  bonus += getTotalChevos();
  return (s_timePerMin * bonus) / 100;
}

/**
 * Redo the calculation about how much time we should be making every min
 * and take into acount all bonuses. Won't overflow
 **/
void updateTimePerMin() {
  s_timePerMin = 60; // This is the base level
  for (uint32_t upgrade = 0; upgrade < MAX_UPGRADES; ++upgrade ) {
    s_timePerMin += getUserUpgrades(REFINERY_ID, upgrade) * UPGRADE_REWARD[REFINERY_ID][upgrade];
  }
}

uint64_t getTankCapacity() {
  // LEGENDARY BONUS. 5%+ tank capacity
  if ( getUserItems(LEGENDARY_ID, TANKCAP_5PERC) == 1 ) {
    return safeMultiply(s_timeCapacity, 21) / 20;
  }
  return s_timeCapacity;
}

/**
 * Total capacity, *COULD OVERFLOW*, devote extra preventative CPU againsts this
 **/
void updateTankCapacity() {
  s_timeCapacity = SEC_IN_HOUR; // Base level
  for (uint32_t upgrade = 0; upgrade < MAX_UPGRADES; ++upgrade ) {
    s_timeCapacity = safeAdd( s_timeCapacity, safeMultiply( UPGRADE_REWARD[TANK_ID][upgrade], getUserUpgrades(TANK_ID, upgrade)) );
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
 * Check that the desired item can be afforded, and buy it if so. Return price paid or zero if fail
 */
uint64_t doPurchase(const uint32_t typeID, const uint32_t resourceID) {
  // Can we buy any more?
  if (getIfAtMaxLevel(typeID, resourceID) == true) return 0;
  uint64_t cost = getPriceOfUpgrade(typeID, resourceID);
  if (cost > getUserTime()) return 0;
  removeTime( cost ); // Debit the user
  addUpgrade(typeID, resourceID, 1); // Give them their upgrade
  // Update the price in the buffer so the next one becomes more expensive
  // And recalculate factors
  s_bufferUpgradePrice[(typeID*MAX_UPGRADES)+resourceID] = cost;
  if (typeID == REFINERY_ID) updateTimePerMin();
  else if (typeID == TANK_ID) updateTankCapacity();
  else if (typeID == WATCHER_ID) updateProbabilities();
  return cost;
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
    snprintf(buffer, buffer_size, "%iy %id", years, days);
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
  if (0 && extraDigits) snprintf(buffer, bufferSize, "%i.%i%%", *value, remain); // totally not working!!! TODO
  else snprintf(buffer, bufferSize, "%i%%", *value);
  return;
}
