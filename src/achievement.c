#include <pebble.h>
#include <limits.h>
#include "achievement.h"
#include "items.h"
#include "persistence.h"
#include "constants.h"
#include "timeStore.h"
#include "timeSieve.h"

//
// ACHIEVEMENT_INCOME_1, // On The Hour, Every Minute
//               ACHIEVEMENT_INCOME_2, //In The Blink of an Eye (civilisation/min)
//               ACHIEVEMENT_TANK_1, // Saving a Rainy Day
//               ACHIEVEMENT_TANK_2,// Age In The Bank [needs better]
//               ACHIEVEMENT_PACK_RAT, // Pack Rat
//               ACHIEVEMENT_ITEM_VALUE, // Estimed Collector
//               ACHIEVEMENT_TANKUP_1, // Moar Capacatah' ()
//               ACHIEVEMENT_TANKUP_2, // Time, All The Way Down
//               ACHIEVEMENT_REFINARYUP_1, // Smoggy
//               ACHIEVEMENT_REFINARYUP_2, // Blighted Landscapes
//               ACHIEVEMENT_WORKERS_1, // Unionize
//               ACHIEVEMENT_WORKERS_2, // Bells and Whistles
//               ACHIEVEMENT_13, //?
//               ACHIEVEMENT_14, //?
//               ACHIEVEMENT_FULL_TANK, //?
//               ACHIEVEMENT_FULL_HISTORY} CHEVOS; //?

#define PACK_RAT_ACHIEVEMENT 1000

uint16_t getTotalUnlockWorkers() {
  uint16_t tot = 0;
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_TECH);
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_LIGHT);
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_FONT);
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_VIBE);
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_COLOUR);
  return tot;
}

uint16_t getTotalItemWorkers() {
  uint16_t tot = 0;
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_FREQUENCY_1);
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_CHANCE_1);
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_QUALITY_1);
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_FREQUENCY_2);
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_CHANCE_2);
  tot += getUserOwnsUpgrades(WATCHER_ID, WATCHER_QUALITY_2);
  return tot;
}

int8_t checkAchievementsInternal() {

  if (getTotalChevos() == MAX_CHEVOS) return -1;

  for (uint8_t a = 0; a < MAX_CHEVOS; ++a) {
    if (getUserChevo(a) == true) continue;
    switch (a) {
      case ACHIEVEMENT_INCOME_1: if (getTimePerMin() >= SEC_IN_HOUR) return a; break;
      case ACHIEVEMENT_INCOME_2: if (getTimePerMin() >= 7000*SEC_IN_YEAR) return a; break;
      case ACHIEVEMENT_TANK_1: if (getUserTime() >= SEC_IN_DAY) return a; break;
      case ACHIEVEMENT_TANK_2: if (getUserTime() >= SEC_IN_AGE) return a; break;
      case ACHIEVEMENT_PACK_RAT: if (getUserGrandTotalItems() >= PACK_RAT_ACHIEVEMENT) return a; break;
      case ACHIEVEMENT_ITEM_VALUE: if (currentTotalSellPrice() >= SEC_IN_EPOCH) return a; break;
      case ACHIEVEMENT_TANKUP_1: if (getUserTotalUpgrades(TANK_ID) >= 200) return a; break;
      case ACHIEVEMENT_TANKUP_2: if (getUserTotalUpgrades(TANK_ID) >= 800) return a; break;
      case ACHIEVEMENT_REFINARYUP_1: if (getUserTotalUpgrades(REFINERY_ID) >= 100) return a; break;
      case ACHIEVEMENT_REFINARYUP_2: if (getUserTotalUpgrades(REFINERY_ID) >= 900) return a; break;
      case ACHIEVEMENT_WORKERS_1: if (getTotalUnlockWorkers() >= 20) return a; break;
      case ACHIEVEMENT_WORKERS_2: if (getTotalItemWorkers() >= 50) return a; break;
      case ACHIEVEMENT_13: break; //?
      case ACHIEVEMENT_14: break; //?
      case ACHIEVEMENT_FULL_TANK: if (getTankCapacity() == ULLONG_MAX) return a; break;
      case ACHIEVEMENT_FULL_HISTORY: if (getUserTime() == ULLONG_MAX) return a; break;
      default: break;
    }
  }

  return -1;
}

/**
 * See if we can award a chevo'
 **/
 int8_t checkAchievements() {
  int8_t result = checkAchievementsInternal();
  if (result > -1) {
    setUserChevo(result, true);
    showNotifyAchievement(result);
  }
  return result;
}
