#include "items.h"
#include "constants.h"
#include "timeSieve.h"
#include "persistence.h"
#include "timeStore.h"
#include "mainWindow.h"

static int32_t s_findChanceBase;
static int32_t s_findChanceMin;
static int32_t s_findChanceHour;
static int32_t s_findChanceDay;
static int32_t s_findChanceMonth;
static int32_t s_findChanceYear;

static int32_t s_qualityChanceBase;

static int32_t s_autoCollectChance;

int32_t getAutoCollectChance() {
  return s_autoCollectChance;
}

int32_t getQualityBaseChance() {
  return s_qualityChanceBase;
}

int32_t getFindBaseChance() {
  return s_findChanceBase;
}


uint32_t combineProbability(uint32_t a, uint32_t b) {
  return (a + b) - (( a * b ) / SCALE_FACTOR);
}

/**
 * Load base probabilities and then increase them based on the user's employees
 **/
void updateProbabilities() {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "SItmBufr");

  s_findChanceBase  = 0;
  s_findChanceMin   = BASE_CHANCE_MIN;
  s_findChanceHour  = BASE_CHANCE_HOUR;
  s_findChanceDay   = BASE_CHANCE_DAY;
  s_findChanceMonth = BASE_CHANCE_MONTH;
  s_findChanceYear  = BASE_CHANCE_YEAR;

  for (uint e = 0; e < getUserOwnsUpgrades(WATCHER_ID, WATCHER_FREQUENCY_1); ++e) {
    s_findChanceBase  = combineProbability(s_findChanceBase, FREQUENCY_1_CHANCE);
    s_findChanceMin   = combineProbability(s_findChanceMin, FREQUENCY_1_CHANCE);
    s_findChanceHour  = combineProbability(s_findChanceHour, FREQUENCY_1_CHANCE);
    s_findChanceDay   = combineProbability(s_findChanceDay, FREQUENCY_1_CHANCE);
    s_findChanceMonth = combineProbability(s_findChanceMonth, FREQUENCY_1_CHANCE);
    s_findChanceYear  = combineProbability(s_findChanceYear, FREQUENCY_1_CHANCE);
  }
  for (uint e = 0; e < getUserOwnsUpgrades(WATCHER_ID, WATCHER_FREQUENCY_2); ++e) {
    s_findChanceBase  = combineProbability(s_findChanceBase, FREQUENCY_2_CHANCE);
    s_findChanceMin   = combineProbability(s_findChanceMin, FREQUENCY_2_CHANCE);
    s_findChanceHour  = combineProbability(s_findChanceHour, FREQUENCY_2_CHANCE);
    s_findChanceDay   = combineProbability(s_findChanceDay, FREQUENCY_2_CHANCE);
    s_findChanceMonth = combineProbability(s_findChanceMonth, FREQUENCY_2_CHANCE);
    s_findChanceYear  = combineProbability(s_findChanceYear, FREQUENCY_2_CHANCE);
  }

  s_qualityChanceBase = 0;
  for (uint e = 0; e < getUserOwnsUpgrades(WATCHER_ID, WATCHER_QUALITY_1); ++e) {
    s_qualityChanceBase = combineProbability(s_qualityChanceBase, QUALITY_1_CHANCE);
  }
  for (uint e = 0; e < getUserOwnsUpgrades(WATCHER_ID, WATCHER_QUALITY_2); ++e) {
    s_qualityChanceBase = combineProbability(s_qualityChanceBase, QUALITY_2_CHANCE);
  }

  s_autoCollectChance = 0;
  for (uint e = 0; e < getUserOwnsUpgrades(WATCHER_ID, WATCHER_CHANCE_1); ++e) {
    s_autoCollectChance = combineProbability(s_autoCollectChance, COLLECTOR_1_CHANCE);
  }
  for (uint e = 0; e < getUserOwnsUpgrades(WATCHER_ID, WATCHER_CHANCE_2); ++e) {
    s_autoCollectChance = combineProbability(s_autoCollectChance, COLLECTOR_2_CHANCE);
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "EItmBufr");

}

/**
 * Get how rare the found item is
 * Also, as we don't want an early player hitting the jackpot, we set a minimum
 * threshold of all-time-time which must be met per category.
 * This is arbitrary, and is currently set to the value of the 3rd most expensive
 * item in the category
 **/
uint8_t getItemRarity(TimeUnits units_changed) {

  // Start with random
  int32_t chance = rand() % SCALE_FACTOR;
  // Add on our minions
  chance = combineProbability(chance, s_qualityChanceBase);
  // Add on the bonus for the time unit
  if ((units_changed & YEAR_UNIT) != 0) {
    chance = combineProbability(chance, QUALITY_YEAR_CHANCE);
  } else if ((units_changed & MONTH_UNIT) != 0) {
    chance = combineProbability(chance, QUALITY_MONTH_CHANCE);
  } else if ((units_changed & DAY_UNIT) != 0) {
    chance = combineProbability(chance, QUALITY_DAY_CHANCE);
  } else if ((units_changed & HOUR_UNIT) != 0) {
    chance = combineProbability(chance, QUALITY_HOUR_CHANCE);
  }

  // LEGENDARY BONUS - 3% item quality bonus
  if ( getUserItems(LEGENDARY_ID, ITEMQ_3PERC) == 1 ) {
    chance = combineProbability(chance, 30);
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "QItm r:%i", (int)chance);
  // Check to see what we win
  if (chance >= BASE_CHANCE_LEGENDARY) return LEGENDARY_ID;
  else if (getUserTotalTime() > SELL_PRICE_EPIC[2] && chance >= BASE_CHANCE_EPIC) return EPIC_ID;
  else if (getUserTotalTime() > SELL_PRICE_RARE[2] && chance >= BASE_CHANCE_RARE) return RARE_ID;
  else if (getUserTotalTime() > SELL_PRICE_MAGIC[2] && chance >= BASE_CHANCE_MAGIC) return MAGIC_ID;
  return COMMON_ID;
}

int32_t getItemAppearChance(TimeUnits units_changed) {
  if      ((units_changed & YEAR_UNIT)  != 0) return s_findChanceYear;
  else if ((units_changed & MONTH_UNIT) != 0) return s_findChanceMonth;
  else if ((units_changed & DAY_UNIT)   != 0) return s_findChanceDay;
  else if ((units_changed & HOUR_UNIT)  != 0) return s_findChanceHour;
  else                                        return s_findChanceMin;
}

bool getItemAppears(TimeUnits units_changed) {
  // We have five different thresholds, min, hour, day, month year. Always use the larger
  int32_t prob = getItemAppearChance(units_changed);
  // LEGENDARY BONUS - 2% item find bonus
  if ( getUserItems(LEGENDARY_ID, ITEMF_2PERC) == 1 ) {
    prob = combineProbability(prob, 20);
  }
  // LEGENDARY BONUS - 10% item find bonus on the hour
  if ( getUserItems(LEGENDARY_ID, ITEMF_HOUR_10PERC) == 1) {
    if ((units_changed & HOUR_UNIT) > 0) prob = combineProbability(prob, 100);
  }
  int r = rand() % SCALE_FACTOR;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "FndItm r:%i thresh:%i P:%i", r, (int)prob, (int)(r<prob));
  return (r < prob);
}

/**
 * Give each hired collector in turn a chance at getting the item
 **/
bool getItemAutoCollect() {
  // LEGENDARY BONUS - 20% auto collect only at midnight
  int32_t chance = s_autoCollectChance;
  if ( getUserItems(LEGENDARY_ID, MIDNIGHT_20PERC) == 1 ) {
    // We set the DAY_UNIT bit to midday as well. But we also unset the second bit to let us
    // know that it's midday not midnight
    if ((getLastTimeUnit() & DAY_UNIT) > 0 && (getLastTimeUnit() & SECOND_UNIT) == 0) {
      chance = combineProbability(chance, 200);
    }
  }
  // LEGENDARY BONUS - 3% auto collect
  if ( getUserItems(LEGENDARY_ID, AUTOCOLLECT_3PERC) == 1 ) {
    chance = combineProbability(chance, 30);
  }
  return ( rand() % SCALE_FACTOR < chance);
}

void genRandomItem(uint8_t* treasureID, uint8_t* itemID) {
  *itemID = rand() % MAX_TREASURES;
  // Legendary is 1/MAX_UNIQUE and there must be one we dont have else downgrade to epic
  if (*treasureID == LEGENDARY_ID) {
    if (getUserTotalItems(LEGENDARY_ID) == MAX_UNIQUE) {
      *treasureID = EPIC_ID;
    } else {
      uint8_t loopBreak = 0;
      while (true) {
        *itemID = rand() % MAX_UNIQUE;
        if (getUserItems(*treasureID, *itemID) == 0) break;
        if (++loopBreak == 100) { // This should never happen! But do not want to risk inf. loop
          *treasureID = EPIC_ID;
          *itemID = rand() % MAX_TREASURES;
          break;
        }
      }
    }
  }
}

/**
 * Check how many we could sell at the current price without filling up the tank.
 * Sell up to that amount
 **/
uint16_t sellItem(const unsigned treasureID, const unsigned itemID, bool sellAll) {
  uint64_t currentPrice = getCurrentSellPrice(treasureID, itemID);
  uint16_t toSell = getUserItems(treasureID, itemID);
  if (sellAll == false) toSell = 1;
  uint64_t capacityLeft = getTankCapacity() - getUserTime();
  uint16_t couldSell = capacityLeft / currentPrice; // Round down is good, no loss of time

  // Don't sell what we cannot store
  if (toSell > couldSell) toSell = couldSell;
  if (toSell == 0) return 0;
  removeItem(treasureID, itemID, toSell);
  addTime( toSell * currentPrice );
  updateDisplayTime( getUserTime() );
  return toSell;
}

int8_t checkForItem(TimeUnits units_changed) {

  if (getItemAppears(units_changed) == false) return -1;

  uint8_t treasureID = getItemRarity(units_changed);
  uint8_t itemID = 0;
  genRandomItem(&treasureID, &itemID); // Both are modified

  displyItem( treasureID, itemID );
  return treasureID;
}

const char* getItemName(uint8_t treasureID, uint8_t itemID) {
  if (treasureID == COMMON_ID) return NAME_COMMON[itemID];
  else if (treasureID == MAGIC_ID) return NAME_MAGIC[itemID];
  else if (treasureID == RARE_ID) return NAME_RARE[itemID];
  else if (treasureID == EPIC_ID) return NAME_EPIC[itemID];
  else return NAME_LEGENDARY[itemID];
 }
