#pragma once
#include <pebble.h>
  
// User's current
uint64_t s_userLTime; 
uint64_t s_userLTimeCapacity;

void init_timeStore();

void time_to_string(uint64_t time, char* buffer);

void percentage_to_string(uint64_t amount, uint64_t total, char* buffer, unsigned* value);

