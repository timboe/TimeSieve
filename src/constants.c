#include <pebble.h>
#include "constants.h"

const uint8_t ITEM_COLOURS[ITEM_CATEGORIES] = {0b11101010, 0b11011000, 0b11001011, 0b11100111, 0b11111000};

const char* const ITEM_CATEGORIE_NAME[ITEM_CATEGORIES] = {"COMMON", "MAGIC", "RARE", "EPIC", "LEGENDARY"};

const uint64_t UPGRADE_PRICE[UPGRADE_CATEGORIES][MAX_UPGRADES] = { {
  60,	//REF
  240,
  768,
  2150,
  7741,
  34062,
  163499,
  555897,
  1556512,
  12452100,
  44827560,
  143448192,
  659861685,
  0,
  0,
  0
  }, {
  SEC_IN_MIN, // TANK
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
  0,//UNUSED
  0,//UNUSED
  0//UNUSED
  }, {
  1, // WATCHER
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  0,//UNUSED
  0,//UNUSED
  0,//UNUSED
  0,//UNUSED
  0//UNUSED
  } };

const uint64_t UPGRADE_REWARD[UPGRADE_CATEGORIES][MAX_UPGRADES] = { {
  20,	// REF
  133,
  905,
  4412,
  18643,
  130877,
  884733,
  5175690,
  84104967,
  437345829,
  1989923523,
  13581228049,
  88277982322,
  0,
  0,
  0
  }, {
  10*SEC_IN_MIN, // TANK
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
  0
  }, {
  SEC_IN_MIN, // WATCHER - TODO remove this - it's not used!
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
  (uint64_t)0
  } };

const char* const UPGRADE_NAME[UPGRADE_CATEGORIES][MAX_UPGRADES] = { {
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
  ""
  }, {
  "Minute Tank",
  "TUPGRADE 2",
  "TUPGRADE 3",
  "TUPGRADE 4",
  "TUPGRADE 5",
  "TUPGRADE 6",
  "Time Butt",
  "TUPGRADE 8",
  "TUPGRADE 9",
  "TUPGRADE 10",
  "TUPGRADE 11",
  "TUPGRADE 12",
  "TUPGRADE 13",
  "",
  "",
  ""
  }, {
  "name here", // Increase treasure chance
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
  ""
  } };

const uint64_t SELL_PRICE[SELLABLE_CATEGORIES][MAX_TREASURES] = { {
  SEC_IN_MIN*10, // COMMON
  SEC_IN_MIN*30,
  SEC_IN_HOUR*2,
  SEC_IN_HOUR*6
  }, {
  SEC_IN_HOUR*18, // MAGIC
  SEC_IN_DAY*5,
  SEC_IN_DAY*20,
  SEC_IN_DAY*30*2
  }, {
  SEC_IN_DAY*31*7, // RARE
  SEC_IN_YEAR*3,
  SEC_IN_YEAR*15,
  SEC_IN_YEAR*75
  }, {
  SEC_IN_YEAR*200, // EPIC
  SEC_IN_YEAR*500,
  SEC_IN_MILLENIUM*1,
  SEC_IN_MILLENIUM*5
  } };

const char* const ITEM_NAME[SELLABLE_CATEGORIES][MAX_TREASURES] = { {
  "Old Tooth",
  "Coathangr",
  "Googly i",
  "Milk"
  }, {
  "Briefs",
  "Rick Roll",
  "Xray Spex",
  "Mm Pizza"
  }, {
  "RARE 1",
  "RARE 2",
  "Bitcoin",
  "Man's Soul"
  }, {
  "EPIC 1",
  "Fine Art",
  "It's a Tiara",
  "Z' Boson"
  } };

const char* const NAME_LEGENDARY[MAX_UNIQUE] = {
  "Antioch",
  "LEG 2",
  "Bat Cookie",
  "Snuke",
  "Flower Power",
  "Puzzle Box",
  "Jolly Roger",
  "Gemerald",
  "Dino Tooth",
  "Loaded Die",
  "Crystal Skull",
  "Diamond Hoe",
  "Gold Horseshoe",
  "Dawkinian",
  "Triforce",
  "LEG 16"};

const char* const DESC_LEGENDARY[MAX_UNIQUE] = {
  "Missed items give a little time",
  "Tank holds 5% more",
  "20% auto-collect bonus at midnight",
  "3% increased item quality",
  "LEG_5",
  "2% bonus to item find",
  "10% item find bonus on the hour",
  "Items sell for 5% more",
  "2% discount on refinery upgrades",
  "3% auto-collect bonus",
  "x3 time bonus on the hour",
  "3% discount on tank upgrades",
  "Markets shift in your favour",
  "5% chance to find two of an item",
  "4% discount on employees",
  "1% time bonus per 100 owned items"};

const char* const NAME_ACHIEVEMENT[MAX_CHEVOS] = {
  "Of the hour",
  "Blink of an eye",
  "Save a rainy day",
  "Age in the bank",
  "Pack rat",
  "Estimed Collector",
  "Need moar space!",
  "Time, all the way down",
  "Smoggy",
  "Blighted landscape",
  "Bells & Whistles",
  "Uinionise",
  "DESC 13",
  "DESC 14",
  "Game over",
  "Uncountable memories"};

const char* const DESC_ACHIEVEMENT[MAX_CHEVOS] = {
  "Earn over an hour per minute",
  "Earn all of human civilisation per minute",
  "Tank capcity of 1 day",
  "Geological tank capacity",
  "Own over 1000 treasures",
  "Own treasure worth an Epoch",
  "Purchase 200 tank upgrades",
  "Purchase 800 tank upgrades",
  "Purchase 100 refinary upgrades",
  "Purchase 900 refinary upgrades",
  "Hire all improvement staff",
  "Hire 50 employees",
  "DESC 13",
  "DESC 14",
  "Fill the maximum capacity time tank",
  "Overflow the global time counter"};
