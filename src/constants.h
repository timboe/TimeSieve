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

#define IS_DEBUG 1

// Used in animations
#define MS_IN_SEC 1000
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
// Watchers increase by a constant factor
#define INCREASE_WATCHER 10

#define MAX_UPGRADES 16
#define MAX_TREASURES 4
#define MAX_UNIQUE 16
#define MAX_CHEVOS 16

#define REFINERY_ID 0
#define TANK_ID 1
#define WATCHER_ID 2

#define COMMON_ID 0
#define MAGIC_ID 1
#define RARE_ID 2
#define EPIC_ID 3
#define LEGENDARY_ID 4
// Only common through epic may be sold on the market
#define SELLABLE_CATEGORIES 4
#define ITEM_CATEGORIES 5

#define TREASURE_DISPLAY_TIME 5000
#define NOTIFY_DISPLAY_TIME 3000
#define SELL_DISPLAY_TIME 3000

#define N_REFINERY_UPGRADES 13
#define N_TANK_UPGRADES 13
#define N_WATCHER_UPGRADES 11

extern const char* const NAME_REFINERY[MAX_UPGRADES];
extern const char* const NAME_TANK[MAX_UPGRADES];
extern const char* const NAME_WATCHER[MAX_UPGRADES];
extern const char* const NAME_COMMON[MAX_TREASURES];
extern const char* const NAME_MAGIC[MAX_TREASURES];
extern const char* const NAME_RARE[MAX_TREASURES];
extern const char* const NAME_EPIC[MAX_TREASURES];
extern const char* const NAME_LEGENDARY[MAX_UNIQUE];
extern const uint64_t INITIAL_PRICE_REFINERY[MAX_UPGRADES];
extern const uint64_t INITIAL_PRICE_TANK[MAX_UPGRADES];
extern const uint64_t INITIAL_PRICE_WATCHER[MAX_UPGRADES];
extern const uint64_t REWARD_REFINERY[MAX_UPGRADES];
extern const uint64_t REWARD_TANK[MAX_UPGRADES];
extern const uint64_t REWARD_WATCHER[MAX_UPGRADES];
extern const uint64_t SELL_PRICE_COMMON[MAX_TREASURES];
extern const uint64_t SELL_PRICE_MAGIC[MAX_TREASURES];
extern const uint64_t SELL_PRICE_RARE[MAX_TREASURES];
extern const uint64_t SELL_PRICE_EPIC[MAX_TREASURES];

typedef enum {WATCHER_FREQUENCY_1,
              WATCHER_TECH,
              WATCHER_CHANCE_1,
              WATCHER_LIGHT,
              WATCHER_QUALITY_1,
              WATCHER_FONT,
              WATCHER_FREQUENCY_2,
              WATCHER_VIBE,
              WATCHER_CHANCE_2,
              WATCHER_COLOUR,
              WATCHER_QUALITY_2,
              W_12,
              W_13,
              W_14,
              W_15,
              W_16} WATCHER_TYPE;

// TREASURE SETTINGS

// Prob max is 1million
#define PROB_MAX 1000000
// Basic collectors have a 1% chance
#define COLLECTOR_1_CHANCE 10000
// Advanced collectors have a 5% chance
#define COLLECTOR_2_CHANCE 50000
// Basic finders have a 0.5% chance to find an item
#define FREQUENCY_1_CHANCE 5000
// Advanced finders have a 3% chance to find an item
#define FREQUENCY_2_CHANCE 30000
// Basic archeologists increase the chance by 1%
// This is done via multiplication so needs a divisor
#define QUALITY_SCALEFACTOR 1000
#define QUALITY_1_CHANCE 10
// Advanced achhelogists increase the chance by 2.5%
#define QUALITY_2_CHANCE 25

// When it comes to finding items, mins have a 0.7% chance (roughly 10 per day)
#define BASE_CHANCE_MIN 6944
// Hours have base chance of 25%
#define BASE_CHANCE_HOUR 250000
// Days have a base chance of 65%
#define BASE_CHANCE_DAY 650000
// Months have a base chance of 90%
#define BASE_CHANCE_MONTH 900000
// Years have a base chance of 99%
#define BASE_CHANCE_YEAR 990000

// Note here the probabilities are done cumulativly
// Common items have a base chance of 70%
// Magic items have a base chance of 20%
#define BASE_CHANCE_MAGIC 800000
// Rare items have a base chance of 7%
#define BASE_CHANCE_RARE 920000
// Epic items have a base chance of 2.5%
#define BASE_CHANCE_EPIC 975000
// Legendary items have a base chance of 0.5%
#define BASE_CHANCE_LEGENDARY 995000

// !TRESSURE SETTINGS

#define MENU_BACK_GREEN_ODD GColorMintGreen
#define MENU_BACK_GREEN_EVEN GColorScreaminGreen
#define MENU_BACK_GREEN_SELECT GColorDarkGreen
#define MENU_BACK_RED_ODD GColorMelon
#define MENU_BACK_RED_EVEN GColorSunsetOrange
#define MENU_BACK_RED_SELECT GColorDarkCandyAppleRed
#define MENU_BACK_BLUE_ODD GColorVividCerulean
#define MENU_BACK_BLUE_EVEN GColorPictonBlue
#define MENU_BACK_BLUE_SELECT GColorOxfordBlue
#define MENU_BACK_YELLOW_ODD GColorIcterine
#define MENU_BACK_YELLOW_EVEN GColorYellow
#define MENU_BACK_YELLOW_SELECT GColorWindsorTan
#define MENU_BACK_PURPLE_ODD GColorPurpureus
#define MENU_BACK_PURPLE_EVEN GColorLavenderIndigo
#define MENU_BACK_PURPLE_SELECT GColorJazzberryJam

#define COLOUR_COMMON GColorLightGray
#define COLOUR_MAGIC GColorKellyGreen
#define COLOUR_RARE GColorVividCerulean
#define COLOUR_EPIC GColorElectricUltramarine
#define COLOUR_LEGENDARY GColorChromeYellow

#define PALETTE_BLUE 0
#define PALETTE_GREEN 1
#define PALETTE_YELLOW 2
#define PALETTE_RED 3
#define PALETTE_MAX 4

#define FONT_1 0
#define FONT_2 1
#define FONT_3 2
#define FONT_4 3
#define FONT_5 4
#define FONT_MAX 5

#define TECH_NONE 0
#define TECH_BATTERY 1
#define TECH_MONTH 2
#define TECH_WEATHER 3
#define TECH_MAX 4

#define NOTIFY_NONE 0
#define NOTIFY_COMMON 1
#define NOTIFY_MAGIC 2
#define NOTIFY_RARE 3
#define NOTIFY_EPIC 4
#define NOTIFY_LEGENDARY 5
#define NOTIFY_MAX 6

#define NUM_BUY_MENU_SECTIONS 1
#define NUM_BUY_MENU_ITEMS 3
#define MENU_CELL_LARGE_HEADER_HEIGHT 32
#define MENU_X_OFFSET 28
#define MENU_CELL_HEIGHT 56
#define MENU_SMALL_CELL_HEIGHT 44

#define MAX_NOTIFY_SETTINGS 6

#define TEXT_BUFFER_SIZE 32
#define TEXT_LARGE_BUFFER_SIZE 64

#define TANK_TEXT_V_OFFSET 5
#define TANK_TEXT_HEIGHT 20

#define CLOCK_TEXT_SIZE 9

// Bits in the liquid
#define N_LIQUID_BITS 20
#define N_WATERFALL_BITS 2
#define WATERFALL_SPEED 5

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
#define ANIM_MIN_V 100
#define ANIM_MAX_V 300
#define ANIM_FRAMES ANIM_FPS*ANIM_DURATION
#define ANIM_DELAY MS_IN_SEC/ANIM_FPS

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

#define N_USER_SETTING 12
typedef enum {SETTING_LIGHT,
              SETTING_VIBE,
              SETTING_TYPE,
              SETTING_COLOUR,
              SETTING_ZZZ_START,
              SETTING_ZZZ_END,
              SETTING_TECH,
              SET_8,
              SET_9,
              SET_10,
              SET_11,
              SET_12} USER_SETTING;

typedef enum {ACHIEVEMENT_1, // hour/min
              ACHIEVEMENT_2, //year/min
              ACHIEVEMENT_3, //millenium/min
              ACHIEVEMENT_4, // store 1 eon
              ACHIEVEMENT_5,// store 1 day 'saving a rainy day'
              ACHIEVEMENT_6, //store human civilisation
              ACHIEVEMENT_7, // pack rat - own 1000 items
              ACHIEVEMENT_8, // own items valued > 1 age
              ACHIEVEMENT_9, // 50 tank upgrades
              ACHIEVEMENT_10, // 200 tank upg
              ACHIEVEMENT_11, // 1000 tank upg
              ACHIEVEMENT_12, // 100 ref up
              ACHIEVEMENT_13, //400 ref up
              ACHIEVEMENT_14, // 1500 ref up
              ACHIEVEMENT_15, //?
              ACHIEVEMENT_16} CHEVOS; //?
