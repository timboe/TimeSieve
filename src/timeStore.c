#include <pebble.h>
#include "timeStore.h"
#include "constants.h"
  
static uint64_t* s_bufferRefineryPrice;
static uint64_t* s_bufferTankPrice;
static uint64_t* s_bufferSievePrice;
static uint64_t* s_bufferWatcherPrice;
  
void init_timeStore() {
  s_userLTime = 7;
  s_userLTimeCapacity = SEC_IN_HOUR;
  
  s_bufferRefineryPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferTankPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferSievePrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
  s_bufferWatcherPrice = (uint64_t*) malloc( MAX_UPGRADES*sizeof(uint64_t) );
}

void destroy_timeStore() {
  free( s_bufferRefineryPrice );
  free( s_bufferTankPrice );
  free( s_bufferSievePrice );
  free( s_bufferWatcherPrice );
}

uint64_t getPriceOfUpgrade(const unsigned typeID, const unsigned resourceID, const unsigned alreadyOwned) {
//   uint64_t basePrice = 0;
//   if (typeID == REFINERY_ID) {
//     basePrice = INITIAL_PRICE_REFINERY[resourceID];
//   } else if (typeID == TANK_ID) {
//     basePrice = INITIAL_PRICE_TANK[resourceID];
//   } else if (typeID == SIEVE_ID) {
//    basePrice = INITIAL_PRICE_SIEVE[resourceID];
//   } else if (typeID == WATCHER_ID) {
//     basePrice = INITIAL_PRICE_WATCHER[resourceID];
//   }
  uint64_t currentPrice = (s_bufferRefineryPrice[resourceID]);
  return 0;
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

