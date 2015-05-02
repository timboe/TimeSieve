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

// DEFINE EXPONENTIAL INCREASE FACTOR, CURRENTLY 7/6 which is ~x1.16
#define INCREASE_MULTIPLY 7
#define INCREASE_DIVIDE 6
  
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
extern const uint64_t REWARD_REFINERY[MAX_UPGRADES];
extern const uint64_t REWARD_SIEVE[MAX_UPGRADES];
extern const uint64_t REWARD_TANK[MAX_UPGRADES];
extern const uint64_t REWARD_WATCHER[MAX_UPGRADES];

#define MENU_BACK_GREEN_ODD GColorMintGreen
#define MENU_BACK_GREEN_EVEN GColorScreaminGreen
#define MENU_BACK_GREEN_SELECT GColorDarkGreen
#define MENU_BACK_RED_ODD GColorMelon
#define MENU_BACK_RED_EVEN GColorSunsetOrange
#define MENU_BACK_RED_SELECT GColorDarkCandyAppleRed

#define MENU_BACK_BLUE_ODD GColorMintGreen
#define MENU_BACK_BLUE_EVEN GColorScreaminGreen
#define MENU_BACK_BLUE_SELECT GColorDarkGreen
#define MENU_BACK_YELLOW_ODD GColorMintGreen
#define MENU_BACK_YELLOW_EVEN GColorScreaminGreen
#define MENU_BACK_YELLOW_SELECT GColorDarkGreen

#define NUM_BUY_MENU_SECTIONS 1
#define NUM_BUY_MENU_ITEMS 4
#define MENU_CELL_LARGE_HEADER_HEIGHT 32
#define MENU_X_OFFSET 28
#define MENU_CELL_HEIGHT 69

#define TEXT_BUFFER_SIZE 32
#define TEXT_LARGE_BUFFER_SIZE 64
  
#define TANK_TEXT_V_OFFSET 5
#define TANK_TEXT_HEIGHT 20
  
#define CLOCK_PIX_OFFSET 2
#define CLOCK_TEXT_SIZE 9

// Bits in the liquid
#define N_LIQUID_BITS 20

// PARTICLE EFFECTS
#define N_SPOOGELET 30 
#define SUB_PIXEL 100

#define WIN_SIZE_X 144
#define WIN_SIZE_Y 168

#define PERSISTENT_VERSION_KEY 0
#define PERSISTENT_USERDATA_KEY 1
#define SCHEMA_VERSION 1

#define ANIM_FPS 25
//#define ANIM_TICK_PER_FRAME 2
#define ANIM_DURATION 2
#define ANIM_RESET 1
#define ANIM_MS_PER_SEC 1000
#define ANIM_MIN_V 100
#define ANIM_MAX_V 300
#define ANIM_FRAMES ANIM_FPS*ANIM_DURATION
#define ANIM_DELAY ANIM_MS_PER_SEC/ANIM_FPS
 
#define N_USER_OPT 16
typedef enum {OPT_SHOW_SECONDS,
              OPT_ANIMATE,
              OPT_3,
              OPT_4,
              OPT_5,
              OPT_6,
              OPT_7,
              OPT_8,
              OPT_9,
              OPT_10,
              OPT_11,
              OPT_12,
              OPT_13,
              OPT_14,
              OPT_15,
              OPT_16} USER_OPT;  