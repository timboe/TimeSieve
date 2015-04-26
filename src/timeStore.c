#include <pebble.h>
#include "timeStore.h"
#include "constants.h"
#include "persistence.h"
  
static uint64_t* s_bufferRefineryPrice;
static uint64_t* s_bufferTankPrice;
static uint64_t* s_bufferSievePrice;
static uint64_t* s_bufferWatcherPrice;
static uint64_t s_timePerMin;
static uint64_t s_timeCapacity;

// Perform fixed point increase in price by floor of 7/6.
uint64_t getPriceOfNext(uint64_t priceOfCurrent) {
  priceOfCurrent *= INCREASE_MULTIPLY;
  return priceOfCurrent / INCREASE_DIVIDE;
}

void init_timeStore() {
  
  s_bufferRefineryPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferTankPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferSievePrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferWatcherPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );

  // Populate the buffer. This could take a little time, do it at the start
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

  updateTimePerMin();
  updateTimeCapacity();
}

void destroy_timeStore() {
  free( s_bufferRefineryPrice );
  free( s_bufferTankPrice );
  free( s_bufferSievePrice );
  free( s_bufferWatcherPrice );
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

void addTime(uint64_t toAdd) {
  setUserTime( getUserTime() + toAdd );
}

void removeTime(uint64_t toSubtract) {
  if (toSubtract > getUserTime()) toSubtract = getUserTime();
  setUserTime( getUserTime() - toSubtract );
}

void multiplyTime(uint64_t factor) {
  setUserTime( getUserTime() * factor );
}

void time_to_string(uint64_t time, char* buffer, size_t buffer_size, bool brief) {
  
  int _eons = time / SEC_IN_EON;
  int _eras = (time % SEC_IN_EON) / SEC_IN_ERA;
  
  if (brief && _eons) {
    snprintf(buffer, buffer_size, "%iEon %iEra", _eons, _eras);
    return;
  }  
  
  int _epochs = (time % SEC_IN_ERA) / SEC_IN_EPOCH;
  
  if (brief && _eras) {
    snprintf(buffer, buffer_size, "%iEra %iEpoch", _eras, _epochs);
    return;
  }  
    
  int _ages = (time % SEC_IN_EPOCH) / SEC_IN_AGE;
  
  if (brief && _epochs) {
    snprintf(buffer, buffer_size, "%iEpoch %iAge", _epochs, _ages);
    return;
  }   
  
  int _mils  =  (time % SEC_IN_AGE) / SEC_IN_MILLENIUM;
  
  if (brief && _ages) {
    snprintf(buffer, buffer_size, "%iAge %iM", _ages, _mils);
    return;
  }  
  
  int _years = (time % SEC_IN_MILLENIUM) / SEC_IN_YEAR;
  
  if (brief && _mils) {
    snprintf(buffer, buffer_size, "%iM %iy", _mils, _years);
    return;
  }
  
  int _days  = (time % SEC_IN_YEAR) / SEC_IN_DAY;
  int _hours = (time % SEC_IN_DAY) / SEC_IN_HOUR;
  
  if (brief && _years) {
    snprintf(buffer, buffer_size, "%iy %id %ih", _years, _days, _hours);
    return;
  } 
  
  int _mins  = (time % SEC_IN_HOUR) / SEC_IN_MIN;
  
  if (brief && _days) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%id %ih %im", _days, _hours, _mins);
    return;
  } 
  
  int _secs  = time % SEC_IN_MIN;
  
  if (brief && _hours) {
    snprintf(buffer, buffer_size, "%ih %im %is", _hours, _mins, _secs);
  } else if (brief && _mins) {
    snprintf(buffer, buffer_size, "%im %is", _mins, _secs);
  } else if (brief) {
    snprintf(buffer, buffer_size, "%is", _secs);
  } 
  return;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Did time %iy %id %ih %im %is", _years, _days, _hours, _mins, _secs);
}

void percentage_to_string(uint64_t amount, uint64_t total, char* buffer, unsigned* value) {
  *value = (amount*100) / total;
  if (*value > 100) *value = 100;
  snprintf(buffer, TEXT_BUFFER_SIZE, "%i%%", *value);
  return;
}

