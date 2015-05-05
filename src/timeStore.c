#include <pebble.h>
#include "timeStore.h"
#include "constants.h"
#include "persistence.h"
  
static uint64_t* s_bufferRefineryPrice;
static uint64_t* s_bufferTankPrice;
static uint64_t* s_bufferSievePrice;
static uint64_t* s_bufferWatcherPrice;

static uint64_t* s_bufferCommonSellPrice;
static uint64_t* s_bufferMagicSellPrice;
static uint64_t* s_bufferRareSellPrice;
static uint64_t* s_bufferEpicSellPrice;

static uint64_t s_timePerMin;
static uint64_t s_displayTime;
static uint64_t s_timeCapacity;

#define INITIAL_PRICE_MODULATION 5

// Perform fixed point increase in price by floor of 7/6.
uint64_t getPriceOfNext(uint64_t priceOfCurrent) {
  priceOfCurrent *= INCREASE_MULTIPLY;
  return priceOfCurrent / INCREASE_DIVIDE;
}

/**
 * Every 1m, we modulate the prices, very basic
 */
void modulateSellPrices() {
  for (unsigned item = 0; item < MAX_TREASURES; ++item ) {
    s_bufferCommonSellPrice[item] += ( (s_bufferCommonSellPrice[item]/(uint64_t)100) * (-10+(rand()%21)) ); 
    s_bufferMagicSellPrice[item]  += ( (s_bufferMagicSellPrice[item]/(uint64_t)100)  * (-10+(rand()%21)) ); 
    s_bufferRareSellPrice[item]   += ( (s_bufferRareSellPrice[item]/(uint64_t)100)   * (-10+(rand()%21)) ); 
    s_bufferEpicSellPrice[item]   += ( (s_bufferEpicSellPrice[item]/(uint64_t)100)   * (-10+(rand()%21)) ); 
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
    s_bufferCommonSellPrice[item] = SELL_PRICE_COMMON[item];
    s_bufferMagicSellPrice[item]  = SELL_PRICE_MAGIC[item];
    s_bufferRareSellPrice[item]   = SELL_PRICE_RARE[item];
    s_bufferEpicSellPrice[item]   = SELL_PRICE_EPIC[item];
  }

  // Do initial permuting
  for (uint16_t n = 0; n < INITIAL_PRICE_MODULATION; ++n) {
    modulateSellPrices();
  }
}

void init_timeStore() {
  
  s_bufferRefineryPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferTankPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferSievePrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferWatcherPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );

  // Populate the buffer. This could take a little time, do it at the start
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Start Buffer");
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
    nOwned = getUserOwnsUpgrades(SIEVE_ID, upgrade);
    currentPrice = INITIAL_PRICE_SIEVE[upgrade];
    for (unsigned i = 0; i < nOwned; ++i) currentPrice = getPriceOfNext(currentPrice);
    s_bufferSievePrice[upgrade] = currentPrice;
    //
    nOwned = getUserOwnsUpgrades(TANK_ID, upgrade);
    currentPrice = INITIAL_PRICE_WATCHER[upgrade];
    for (unsigned i = 0; i < nOwned; ++i) currentPrice = getPriceOfNext(currentPrice);
    s_bufferWatcherPrice[upgrade] = currentPrice;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Fin Buffer");

  updateTimePerMin();
  updateTankCapacity();

  initiateSellPrices();
}

void destroy_timeStore() {
  free( s_bufferRefineryPrice );
  free( s_bufferTankPrice );
  free( s_bufferSievePrice );
  free( s_bufferWatcherPrice );

  free( s_bufferCommonSellPrice );
  free( s_bufferMagicSellPrice );
  free( s_bufferRareSellPrice );
  free( s_bufferEpicSellPrice );  
}


uint64_t getPriceOfUpgrade(const unsigned typeID, const unsigned resourceID) {
  uint64_t currentPrice = 0;
  if (typeID == REFINERY_ID) {
    currentPrice = s_bufferRefineryPrice[resourceID];
  } else if (typeID == TANK_ID) {
    currentPrice = s_bufferTankPrice[resourceID];
  } else if (typeID == SIEVE_ID) {
    currentPrice = s_bufferSievePrice[resourceID];
  } else if (typeID == WATCHER_ID) {
    currentPrice = s_bufferWatcherPrice[resourceID];
  }

  return getPriceOfNext(currentPrice);
}

uint64_t getTimePerMin() {
  return s_timePerMin;
}

/**
 * Redo the calculation about how much time we should be making every min
 * and take into acount all bonuses  
 */ 
void updateTimePerMin() {
  s_timePerMin = 60; // This is the base level
  for (unsigned upgrade = 0; upgrade < MAX_UPGRADES; ++upgrade ) {
    s_timePerMin += getUserOwnsUpgrades(REFINERY_ID, upgrade) * REWARD_REFINERY[upgrade];
  }
}

uint64_t getTankCapacity() {
  return s_timeCapacity;
}

void updateTankCapacity() {
  s_timeCapacity = SEC_IN_HOUR; // Base level
  for (unsigned upgrade = 0; upgrade < MAX_UPGRADES; ++upgrade ) {
    s_timeCapacity += getUserOwnsUpgrades(TANK_ID, upgrade) * REWARD_TANK[upgrade];
  }
}

uint64_t getDisplayTime() {
  return s_displayTime;
}

void updateDisplayTime(uint64_t t) {
  s_displayTime = t;
}

void addTime(uint64_t toAdd) {
  if ( getUserTime() + toAdd > getTankCapacity() ) toAdd = getTankCapacity() - getUserTime();
  setUserTime( getUserTime() + toAdd );
  setUserTotalTime( getUserTotalTime() + toAdd );
}

void removeTime(uint64_t toSubtract) {
  if (toSubtract > getUserTime()) toSubtract = getUserTime();
  setUserTime( getUserTime() - toSubtract );
}

 // This is for debug - it doesn´t check if there is space to store the extra
void multiplyTime(uint64_t factor) {
  setUserTime( getUserTime() * factor );
}

/**
 * Check that the desired item can be afforded, and buy it if so
 */
bool doPurchase(const unsigned typeID, const unsigned resourceID) {
  uint64_t cost = getPriceOfUpgrade(typeID, resourceID);
  if (cost > getUserTime()) return false;
  removeTime( cost ); // Debit the user
  addUpgrade(typeID, resourceID); // Give them their upgrade
  // Update the price in the buffer so the next one becomes more expensive
  // And recalculate factors
  if (typeID == REFINERY_ID) {
    s_bufferRefineryPrice[resourceID] = cost;
    updateTimePerMin();
  } else if (typeID == TANK_ID) {
    s_bufferTankPrice[resourceID] = cost;
    updateTankCapacity();
  } else if (typeID == SIEVE_ID) {
    s_bufferSievePrice[resourceID] = cost;
  } else if (typeID == WATCHER_ID) {
    s_bufferWatcherPrice[resourceID] = cost;
  }
  return true;
}


void timeToString(uint64_t time, char* buffer, size_t buffer_size, bool brief) {
  
  int eons = time / SEC_IN_EON;
  int eras = (time % SEC_IN_EON) / SEC_IN_ERA;
  
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
    snprintf(buffer, TEXT_BUFFER_SIZE, "%id %ih %im", days, hours, mins);
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
  if (eons) snprintf(buffer, TEXT_BUFFER_SIZE, "%iEon %iEra %iEpoch %iAge %iM %iy %id %ih %im %is", eons, eras, epochs, ages, mills, years, days, hours, mins, secs); 
  else if (eras) snprintf(buffer, TEXT_BUFFER_SIZE, "%iEra %iEpoch %iAge %iM %iy %id %ih %im %is", eras, epochs, ages, mills, years, days, hours, mins, secs); 
  else if (epochs) snprintf(buffer, TEXT_BUFFER_SIZE, "%iEpoch %iAge %iM %iy %id %ih %im %is", epochs, ages, mills, years, days, hours, mins, secs); 
  else if (ages) snprintf(buffer, TEXT_BUFFER_SIZE, "%iAge %iM %iy %id %ih %im %is", ages, mills, years, days, hours, mins, secs); 
  else if (mills) snprintf(buffer, TEXT_BUFFER_SIZE, "%iM %iy %id %ih %im %is", mills, years, days, hours, mins, secs); 
  else if (years) snprintf(buffer, TEXT_BUFFER_SIZE, "%iy %id %ih %im %is", years, days, hours, mins, secs); 
  else if (days) snprintf(buffer, TEXT_BUFFER_SIZE, "%id %ih %im %is", days, hours, mins, secs); 
  else if (hours) snprintf(buffer, TEXT_BUFFER_SIZE, "%ih %im %is", hours, mins, secs);
  else if (mins) snprintf(buffer, buffer_size, "%im %is", mins, secs);
  else snprintf(buffer, buffer_size, "%is", secs);
  return;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Did time %iy %id %ih %im %is", _years, days, hours, mins, secs);
}

void percentageToString(uint64_t amount, uint64_t total, char* buffer, unsigned* value) {
  *value = (amount*100) / total;
  if (*value > 100) *value = 100;
  snprintf(buffer, TEXT_BUFFER_SIZE, "%i%%", *value);
  return;
}

