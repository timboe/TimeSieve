#include "items.h"
#include "constants.h"
#include "timeSieve.h"
#include "persistence.h"




/*
#define SCALE_FACTOR 10000


uint32_t FPMultiply(const uint32_t a, const uint32_t b) {
  return (a * b )/(SCALE_FACTOR * SCALE_FACTOR);
}

uint32_t getBinomalProb(const uint16_t nAttempts, const uint32_t chance) {
  // P = 1-(1-chance)^attempts
  if (nAttempts == 0) return 0;
  
  // (1-chance)
  uint32_t c = PROB_MAX - chance;
  
  // C = c^attempts
  --nAttempts; // If attempts is 1 then ^1 so don't need this loop
  uint32_t C = c;
  for (uint8_t a = 0; a < nAttempts; ++a) {
    C = FPMultiply(C, c);
  }
  
  // P = 1 - C
  return PROB_MAX - C;
}
*/

bool getChance(uint16_t nTries, int32_t prob) {
  for (uint16_t t = 0; t < nTries; ++t) {
    if ( rand() % PROB_MAX < prob) return true;
  }
  return false;
}

/**
 * Get how rare the found item is - this works a bit differently from the other chances
 * which are binomial.
 * Also, as we don't want an early player hitting the jackpot, we set a minimum
 * threshold of all-time-time which must be met per category. 
 * This is arbitrary, and is currently set to the value of the 3rd most expensive
 * item in the category
 **/
uint8_t getItemRarity(TimeUnits units_changed) {
  uint32_t chance = rand() % PROB_MAX;
  uint32_t q1 = getUserOwnsUpgrades(WATCHER_ID, WATCHER_QUALITY_1);
  uint32_t q2 = getUserOwnsUpgrades(WATCHER_ID, WATCHER_QUALITY_2);
  chance *= QUALITY_SCALEFACTOR + (q1 * QUALITY_1_CHANCE) + (q2 * QUALITY_2_CHANCE);
  chance /= QUALITY_SCALEFACTOR;
  // Increased chance at larger boundaries
  if ((units_changed & YEAR_UNIT) != 0)       chance += 900000;
  else if ((units_changed & MONTH_UNIT) != 0) chance += 500000;
  else if ((units_changed & DAY_UNIT) != 0)   chance += 100000;
  // Check to see what we win
  if (chance > BASE_CHANCE_LEGENDARY) return LEGENDARY_ID;
  else if (getUserTotalTime() > SELL_PRICE_EPIC[2] && chance > BASE_CHANCE_EPIC) return EPIC_ID;
  else if (getUserTotalTime() > SELL_PRICE_RARE[2] && chance > BASE_CHANCE_RARE) return RARE_ID;
  else if (getUserTotalTime() > SELL_PRICE_MAGIC[2] && chance > BASE_CHANCE_MAGIC) return MAGIC_ID;
  return COMMON_ID;
}

bool getItemAppears(TimeUnits units_changed) {
  // We have five different thresholds, min, hour, day, month year. Always use the larger
  if ((units_changed & YEAR_UNIT) != 0) {
    if (rand() % PROB_MAX < BASE_CHANCE_YEAR) return true;
  } else if ((units_changed & MONTH_UNIT) != 0) {
    if (rand() % PROB_MAX < BASE_CHANCE_MONTH) return true;
  } else if ((units_changed & DAY_UNIT) != 0) {
    if (rand() % PROB_MAX < BASE_CHANCE_DAY) return true;
  } else if ((units_changed & HOUR_UNIT) != 0) {
    if (rand() % PROB_MAX < BASE_CHANCE_HOUR) return true;
  } else {
    if (rand() % PROB_MAX < BASE_CHANCE_MIN) return true;
  } 
  // No? OK, try w followers
  return ( getChance(getUserOwnsUpgrades(WATCHER_ID, WATCHER_FREQUENCY_1), FREQUENCY_1_CHANCE) ||
           getChance(getUserOwnsUpgrades(WATCHER_ID, WATCHER_FREQUENCY_2), FREQUENCY_2_CHANCE) );
}

/**
 * Give each hired collector in turn a chance at getting the item
 **/
bool getItemAutoCollect() {
  return ( getChance(getUserOwnsUpgrades(WATCHER_ID, WATCHER_CHANCE_1), COLLECTOR_1_CHANCE) ||
           getChance(getUserOwnsUpgrades(WATCHER_ID, WATCHER_CHANCE_2), COLLECTOR_2_CHANCE) );
}


void checkForItem(TimeUnits units_changed) {

  if (getItemAppears(units_changed) == false) return;

  uint8_t treasureID = getItemRarity(units_changed);
  uint8_t itemID = rand() % MAX_TREASURES;
  
  // Legendary is 1/MAX_UNIQUE and there must be one we dont have else downgrade to epic
  if (treasureID == LEGENDARY_ID) {
    if (getUserTotalItems(LEGENDARY_ID) == MAX_UNIQUE) {
      treasureID = EPIC_ID;
    } else {
      uint8_t loopBreak = 0;
      while (true) {
        itemID = rand() % MAX_UNIQUE;
        if (getUserItems(treasureID, itemID) == 0) break;
        if (++loopBreak == 100) { // This should never happen! But do not want to risk inf. loop
          treasureID = EPIC_ID;
          itemID = rand() % MAX_TREASURES;
          break;
        }
      }
    }
  }

  displyItem( treasureID, itemID );
}
