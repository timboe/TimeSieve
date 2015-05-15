#include <pebble.h>
#include "constants.h"

const uint64_t INITIAL_PRICE_REFINERY[MAX_UPGRADES] = {
  SEC_IN_MIN,
  3*SEC_IN_MIN,
  12*SEC_IN_MIN,
  1*SEC_IN_HOUR,
  6*SEC_IN_HOUR,
  2*SEC_IN_DAY,
  8*SEC_IN_DAY,
  126*SEC_IN_DAY,
  800*SEC_IN_DAY,
  20*SEC_IN_YEAR,
  500*SEC_IN_YEAR,
  2*SEC_IN_MILLENIUM,
  10*SEC_IN_MILLENIUM,
  0,
  0,
  0};

const uint64_t REWARD_REFINERY[MAX_UPGRADES] = {
  1,
  30,
  3*SEC_IN_MIN,
  15*SEC_IN_MIN,
  1*SEC_IN_HOUR,
  6*SEC_IN_HOUR,
  1*SEC_IN_DAY,
  14*SEC_IN_DAY,
  90*SEC_IN_DAY,
  5*SEC_IN_YEAR,
  90*SEC_IN_YEAR,
  666*SEC_IN_YEAR,
  1*SEC_IN_MILLENIUM,
  0,
  0,
  0};

const char* const NAME_REFINERY[MAX_UPGRADES] = {
  "Time Tongs",
  "30 SEC",
  "Funnel",
  "Extra Pumps",
  "1 HOUR",
  "FEW HOUR",
  "DAY",
  "Fortnighter",
  "Season Net",
  "Temporal Sink",
  "Life Leech",
  "Odd Pentagram",
  "Prismatic Gem",
  "",
  "",
  ""};

const uint64_t INITIAL_PRICE_TANK[MAX_UPGRADES] = {
  SEC_IN_MIN,
  30*SEC_IN_MIN,
  5*SEC_IN_HOUR,
  10*SEC_IN_DAY,
  120*SEC_IN_DAY,
  33*SEC_IN_YEAR,
  250*SEC_IN_YEAR,
  3*SEC_IN_MILLENIUM,
  300*SEC_IN_MILLENIUM,
  1*SEC_IN_AGE,
  1*SEC_IN_EPOCH,
  3*SEC_IN_EPOCH,
  1*SEC_IN_ERA,
  0,
  0,
  0};

const uint64_t REWARD_TANK[MAX_UPGRADES] = {
  10*SEC_IN_MIN,
  1*SEC_IN_HOUR,
  12*SEC_IN_HOUR,
  1*SEC_IN_DAY,
  31*SEC_IN_DAY,
  1*SEC_IN_YEAR,
  100*SEC_IN_YEAR,
  1*SEC_IN_MILLENIUM,
  200*SEC_IN_MILLENIUM,
  1*SEC_IN_AGE,
  1*SEC_IN_EPOCH,
  1*SEC_IN_ERA,
  2*SEC_IN_EON,
  0,
  0,
  0};

const char* const NAME_TANK[MAX_UPGRADES] = {
  "Minute Tank",
  "TUPGRADE 2",
  "TUPGRADE 3",
  "TUPGRADE 4",
  "TUPGRADE 5",
  "TUPGRADE 6",
  "TUPGRADE 7",
  "TUPGRADE 8",
  "TUPGRADE 9",
  "TUPGRADE 10",
  "TUPGRADE 11",
  "TUPGRADE 12",
  "TUPGRADE 13",
  "",
  "",
  ""};

const uint64_t INITIAL_PRICE_WATCHER[MAX_UPGRADES] = {
  SEC_IN_MIN,
  SEC_IN_MIN*2,
  SEC_IN_MIN*10,
  SEC_IN_MIN*60,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0};

// remember these prices x10 each time and some are finite
const uint64_t REWARD_WATCHER[MAX_UPGRADES] = {
  SEC_IN_MIN,
  SEC_IN_MIN*2,
  SEC_IN_MIN*10,
  SEC_IN_MIN,
  SEC_IN_MIN,
  SEC_IN_MIN,
  SEC_IN_MIN,
  SEC_IN_MIN,
  SEC_IN_MIN,
  SEC_IN_MIN,
  SEC_IN_MIN,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0,
  (uint64_t)0};

const char* const NAME_WATCHER[MAX_UPGRADES] = {
  "", // Increase treasure chance
  "Techie", // Techie - unlock extra clock bits
  "Intern Runner",// XXX - WATCHER_CHANCE_1
  "Electrician",  // Electrician - light notify
  "Archaeologist", // WATCHER_QUALITY_1
  "Typographer", // Typogrgrapher - unlock font
  "Bounty Hunter", // WATCHER_FREQUENCY_2,
  "Drill Operator", // WATCHER_VIB Craftsman - vibrate notify
  "Sniffer Dog", //WATCHER_CHANCE_2
  "Painter", // Painter - unlock theme
  "Professor", // WATCHER_QUALITY_2 better treasure
  "",
  "",
  "",
  "",
  ""};

const uint64_t SELL_PRICE_COMMON[MAX_TREASURES] = {
  SEC_IN_MIN*10,
  SEC_IN_MIN*30,
  SEC_IN_HOUR*2,
  SEC_IN_HOUR*6};

const uint64_t SELL_PRICE_MAGIC[MAX_TREASURES] = {
  SEC_IN_HOUR*18,
  SEC_IN_DAY*5,
  SEC_IN_DAY*20,
  SEC_IN_DAY*30*2};

const uint64_t SELL_PRICE_RARE[MAX_TREASURES] = {
  SEC_IN_DAY*31*7,
  SEC_IN_YEAR*3,
  SEC_IN_YEAR*15,
  SEC_IN_YEAR*75};

const uint64_t SELL_PRICE_EPIC[MAX_TREASURES] = {
  SEC_IN_YEAR*200,
  SEC_IN_YEAR*500,
  SEC_IN_MILLENIUM*1,
  SEC_IN_MILLENIUM*5};

const char* const NAME_COMMON[MAX_TREASURES] = {
  "Old Tooth",
  "Coathangr",
  "Googly i",
  "Milk"};

const char* const NAME_MAGIC[MAX_TREASURES] = {
  "Briefs",
  "Rick Roll",
  "MAGIC 3",
  "MAGIC 4"};

const char* const NAME_RARE[MAX_TREASURES] = {
  "RARE 1",
  "RARE 2",
  "1 Bitcoin",
  "Man's Soul"};

const char* const NAME_EPIC[MAX_TREASURES] = {
  "EPIC 1",
  "Gemerald",
  "EPIC 3",
  "EPIC 4"};

const char* const NAME_LEGENDARY[MAX_UNIQUE] = {
  "LEG 1",
  "LEG 2",
  "LEG 3",
  "LEG 4",
  "LEG 5",
  "LEG 6",
  "LEG 7",
  "LEG 8",
  "LEG 9",
  "LEG 10",
  "LEG 11",
  "LEG 12",
  "LEG 13",
  "LEG 14",
  "LEG 15",
  "LEG 16"};

const char* const DESC_LEGENDARY[MAX_UNIQUE] = {
  "DESC 1",
  "DESC 2",
  "DESC 3",
  "DESC 4",
  "DESC 5",
  "DESC 6",
  "DESC 7",
  "DESC 8",
  "DESC 9",
  "DESC 10",
  "DESC 11",
  "DESC 12",
  "DESC 13",
  "DESC 14",
  "DESC 15",
  "DESC 16"};


const char* const NAME_ACHIEVEMENT[MAX_CHEVOS] = {
  "DESC 1",
  "DESC 2",
  "DESC 3",
  "DESC 4",
  "DESC 5",
  "DESC 6",
  "DESC 7",
  "DESC 8",
  "DESC 9",
  "DESC 10",
  "DESC 11",
  "DESC 12",
  "DESC 13",
  "DESC 14",
  "DESC 15",
  "DESC 16"};


const char* const DESC_ACHIEVEMENT[MAX_CHEVOS] = {
  "DESC 1",
  "DESC 2",
  "DESC 3",
  "DESC 4",
  "DESC 5",
  "DESC 6",
  "DESC 7",
  "DESC 8",
  "DESC 9",
  "DESC 10",
  "DESC 11",
  "DESC 12",
  "DESC 13",
  "DESC 14",
  "DESC 15",
  "DESC 16"};
