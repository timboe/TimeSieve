#include "items.h"
#include "constants.h"
#include "timeSieve.h"
#include "persistence.h"

uint8_t getItemRarity() {
  // For now totally random
  return rand() % ITEM_CATEGORIES;
}


void checkForItem(TimeUnits units_changed) {

  uint8_t treasureID = getItemRarity();
  uint8_t itemID = rand() % MAX_TREASURES;
  
  // Legendary is 1/MAX_UNIQUE and must be one we dont have else downgrade to epic
  if (treasureID == LEGENDARY_ID) {
    if (getUserTotalItems(LEGENDARY_ID) == MAX_UNIQUE) {
      treasureID = EPIC_ID;
    } else {
      while (true) {
        itemID = rand() % MAX_UNIQUE;
        if (getUserItems(treasureID, itemID) == 0) break;
      }
    }
  }

  displyItem( treasureID, itemID );
}
