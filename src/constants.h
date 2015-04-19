#pragma once
#include <pebble.h>

// DEFINE DIFFERENT TIME PERIODS
#define SEC_IN_MIN 60
#define SEC_IN_HOUR 3600
#define SEC_IN_DAY 86400
#define SEC_IN_YEAR 31536000
// Note this 32 bit unsigned can hold only 136 years
#define SEC_IN_MILLENIUM (uint64_t)31536000000
// Now we're getting geological. We take an age to be 1 million years
#define SEC_IN_AGE (uint64_t)31536000000000
// An epoch is taken as 50 million years
#define SEC_IN_EPOCH (uint64_t)1576800000000000
// An era is taken as 300 million years
#define SEC_IN_ERA (uint64_t)7884000000000000
// An eon is defined as 1 billion years
#define SEC_IN_EON (uint64_t)31536000000000000
// Warning - 584 billion years is the maximum size of uint64_t  
  
  
#define TEXT_BUFFER_SIZE 32
  
#define TANK_TEXT_V_OFFSET 5
#define TANK_TEXT_HEIGHT 20
  
// Bits in the liquid
#define N_LIQUID_BITS 10


#define WIN_SIZE_X 144
#define WIN_SIZE_Y 168
  