#pragma once
#include <pebble.h>
#include <limits.h>		/* for CHAR_BIT */

// Macros for BIT opertaions: http://c-faq.com/misc/bitsets.html
#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

// DEFINE DIFFERENT TIME PERIODS
#define SEC_IN_MIN (uint64_t)60
#define SEC_IN_HOUR (uint64_t)3600
#define SEC_IN_DAY (uint64_t)86400
#define SEC_IN_YEAR (uint64_t)31536000
// Note a 32 bit unsigned can hold only 136 years
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
  
#define MAX_UPGRADES 16
#define MAX_TREASURES 16
#define MAX_UNIQUE 16
  
#define REFINERY_ID 0
#define SIEVE_ID 1
#define TANK_ID 2
#define WATCHER_ID 3
  
#define N_REFINERY_UPGRADES 10
#define N_SIEVE_UPGRADES 10
#define N_TANK_UPGRADES 10
#define N_WATCHER_UPGRADES 4
  
extern const char* const NAME_REFINERY[MAX_UPGRADES];
extern const char* const NAME_SIEVE[MAX_UPGRADES];
extern const char* const NAME_TANK[MAX_UPGRADES];
extern const char* const NAME_WATCHER[MAX_UPGRADES];
extern const uint64_t INITIAL_PRICE_REFINERY[MAX_UPGRADES];
extern const uint64_t INITIAL_PRICE_SIEVE[MAX_UPGRADES];
extern const uint64_t INITIAL_PRICE_TANK[MAX_UPGRADES];
extern const uint64_t INITIAL_PRICE_WATCHER[MAX_UPGRADES];

// typedef enum {
//   TimeTongs, TYPE2, TYPE3, TYPE4,
//   TYPE5, TYPE6, TYPE7, TYPE8,
//   TYPE9, TYPE10, TYPE11, TYPE12,
//   TYPE13, TYPE14, TYPE15, TYPE16,
// } TYPE_REFINERY;

// typedef enum {
//   STYPE1, STYPE2, STYPE3, STYPE4,
//   STYPE5, STYPE6, STYPE7, STYPE8,
//   STYPE9, STYPE10, STYPE11, STYPE12,
//   STYPE13, STYPE14, STYPE15, STYPE16
// } TYPE_SIEVE;

// typedef enum {
//   TTYPE1, TTYPE2, TTYPE3, TTYPE4,
//   TTYPE5, TTYPE6, TTYPE7, TTYPE8,
//   TTYPE9, TTYPE10, TTYPE11, TTYPE12,
//   TTYPE13, TTYPE14, TTYPE15, TTYPE16
// } TYPE_TANK;

// typedef enum {
//   WTYPE1, WTYPE2, WTYPE3, WTYPE4,
//   WTYPE5, WTYPE6, WTYPE7, WTYPE8,
//   WTYPE9, WTYPE10, WTYPE11, WTYPE12,
//   WTYPE13, WTYPE14, WTYPE15, WTYPE16
// } TYPE_WATCHER;

#define BUY_MENU_BACK_COLOUR_ODD GColorMintGreen
#define BUY_MENU_BACK_COLOUR_EVEN GColorSpringBud
#define BUY_MENU_BACK_COLOUR_SELECT GColorDarkGreen

#define TEXT_BUFFER_SIZE 32
#define TEXT_LARGE_BUFFER_SIZE 64
  
#define TANK_TEXT_V_OFFSET 5
#define TANK_TEXT_HEIGHT 20
  
// Bits in the liquid
#define N_LIQUID_BITS 20

#define WIN_SIZE_X 144
#define WIN_SIZE_Y 168
  