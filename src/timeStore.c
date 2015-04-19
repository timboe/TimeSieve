#include <pebble.h>
#include "timeStore.h"
#include "constants.h"
  
void init_timeStore() {
  s_userLTime = 7;
  s_userLTimeCapacity = SEC_IN_HOUR;
}

void time_to_string(uint64_t time, char* buffer) {
  
  int _eons = time / SEC_IN_EON;
  int _eras = (time % SEC_IN_EON) / SEC_IN_ERA;
  
  if (_eons) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%iEon %iEra", _eons, _eras);
    return;
  }  
  
  int _epochs = (time % SEC_IN_ERA) / SEC_IN_EPOCH;
  
  if (_eras) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%iEra %iEpoch", _eras, _epochs);
    return;
  }  
    
  int _ages = (time % SEC_IN_EPOCH) / SEC_IN_AGE;
  
  if (_epochs) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%iEpoch %iAge", _epochs, _ages);
    return;
  }   
  
  int _mils  =  (time % SEC_IN_AGE) / SEC_IN_MILLENIUM;
  
  if (_ages) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%iAge %iM", _ages, _mils);
    return;
  }  
  
  int _years = (time % SEC_IN_MILLENIUM) / SEC_IN_YEAR;
  
  if (_mils) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%iM %iy", _mils, _years);
    return;
  }
  
  int _days  = (time % SEC_IN_YEAR) / SEC_IN_DAY;
  int _hours = (time % SEC_IN_DAY) / SEC_IN_HOUR;
  
  if (_years) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%iy %id %ih", _years, _days, _hours);
    return;
  } 
  
  int _mins  = (time % SEC_IN_HOUR) / SEC_IN_MIN;
  
  if (_days) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%id %ih %im", _days, _hours, _mins);
    return;
  } 
  
  int _secs  = time % SEC_IN_MIN;
  
  if (_hours) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%ih %im %is", _hours, _mins, _secs);
  } else if (_mins) {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%im %is", _mins, _secs);
  } else {
    snprintf(buffer, TEXT_BUFFER_SIZE, "%is", _secs);
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

