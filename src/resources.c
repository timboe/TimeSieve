#include <pebble.h>
#include "resources.h"
#include "persistence.h"
#include "constants.h"

static GFont s_perfectDOSFont;
static bool s_clockLoaded = false;
static GFont s_clock;
static GFont s_clockSmall;
static GFont s_weatherFont;
static GBitmap* s_singleItemImage = NULL;
static GBitmap* s_achievementImage[N_CHEVO_IMAGE];
static GBitmap* s_gem[ITEM_CATEGORIES];

static GBitmap* s_commonItem[MAX_TREASURES];
static GBitmap* s_magicItem[MAX_TREASURES];
static GBitmap* s_rareItem[MAX_TREASURES];
static GBitmap* s_epicItem[MAX_TREASURES];
static GBitmap* s_legendaryItem[MAX_UNIQUE];

static GBitmap* s_refineryImage[MAX_UPGRADES];
static GBitmap* s_tankImage[MAX_UPGRADES];
static GBitmap* s_employeeImage[MAX_UPGRADES];

//////////////////////////

void initMainWindowRes() {
  s_perfectDOSFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PERFECT_DOS_21));
  s_weatherFont    = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_WEATHER_24));
  loadClockFont();
  s_achievementImage[0] = NULL; //TODO
  s_achievementImage[1] = NULL; //TODO
  s_gem[COMMON_ID]    = gbitmap_create_with_resource(RESOURCE_ID_GEM_COMMON);
  s_gem[MAGIC_ID]     = gbitmap_create_with_resource(RESOURCE_ID_GEM_MAGIC);
  s_gem[RARE_ID]      = gbitmap_create_with_resource(RESOURCE_ID_GEM_RARE);
  s_gem[EPIC_ID]      = gbitmap_create_with_resource(RESOURCE_ID_GEM_EPIC);
  s_gem[LEGENDARY_ID] = gbitmap_create_with_resource(RESOURCE_ID_GEM_LEGENDARY);
}

void deinitMainWindowRes() {
  fonts_unload_custom_font(s_perfectDOSFont);
  fonts_unload_custom_font(s_clock);
  fonts_unload_custom_font(s_clockSmall);
  fonts_unload_custom_font(s_weatherFont);
  s_clockLoaded = false;
  gbitmap_destroy( s_singleItemImage );
  s_singleItemImage = NULL;
  gbitmap_destroy(s_achievementImage[0]);
  gbitmap_destroy(s_achievementImage[1]);
  for (uint8_t i = 0; i < ITEM_CATEGORIES; ++i) {
    gbitmap_destroy(s_gem[i]);
  }
}

void loadClockFont() {
  if (s_clockLoaded == true) {
    fonts_unload_custom_font(s_clock);
    fonts_unload_custom_font(s_clockSmall);
  }
  const uint8_t setting = getUserSetting(SETTING_TYPE);
  if (setting == FONT_1) {
    s_clock          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_31));
    s_clockSmall     = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_18));
  } else if (setting == FONT_2) {
    s_clock          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_31));
    s_clockSmall     = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_18));
  } else if (setting == FONT_3) {
    s_clock          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_31));
    s_clockSmall     = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_18));
  } else if (setting == FONT_4) {
    s_clock          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_31));
    s_clockSmall     = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_18));
  } else { // FONT_5
    s_clock          = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_31));
    s_clockSmall     = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_A_18));
  }
  s_clockLoaded = true;
}

GFont* getDOSFont() { return &s_perfectDOSFont; }

GFont* getClockFont() { return &s_clock; }

GFont* getClockSmallFont() { return &s_clockSmall; }

GFont* getWeatherFont() { return &s_weatherFont; }

GBitmap* getSingleItemImage(uint8_t treasureID, uint8_t itemID) {
  gbitmap_destroy( s_singleItemImage );
  s_singleItemImage = loadItemImage(treasureID, itemID);
  return s_singleItemImage;
}

GBitmap* getAchievementImage(uint8_t achievementID) { return s_achievementImage[achievementID]; }

GBitmap* getGemImage(uint8_t treasureID) { return s_gem[treasureID]; }

//////////////////////////

void initSellWindowRes() {
  for (uint8_t i=0; i < MAX_TREASURES; ++i) {
    s_commonItem[i] = loadItemImage(COMMON_ID, i);
    s_magicItem[i]  = loadItemImage(MAGIC_ID, i);
    s_rareItem[i]   = loadItemImage(RARE_ID, i);
    s_epicItem[i]   = loadItemImage(EPIC_ID, i);
  }
}

void deinitSellWindowRes() {
  for (uint8_t i=0; i < MAX_TREASURES; ++i) {
    gbitmap_destroy( s_commonItem[i] );
    gbitmap_destroy( s_magicItem[i] );
    gbitmap_destroy( s_rareItem[i] );
    gbitmap_destroy( s_epicItem[i] );
  }
}

GBitmap* getSellItemImage(uint8_t treasureID, uint8_t itemID) {
  switch (treasureID) {
    case COMMON_ID: return s_commonItem[itemID];
    case MAGIC_ID: return s_magicItem[itemID];
    case RARE_ID: return s_rareItem[itemID];
    case EPIC_ID: return s_epicItem[itemID];
  }
  return NULL;
}

// This internal fn is used elsewhere too in resources
GBitmap* loadItemImage(const uint8_t treasureID, const uint8_t itemID) {
  if (treasureID == COMMON_ID) {
    switch (itemID) {
      case 0: return gbitmap_create_with_resource(RESOURCE_ID_COMMON_0);
      case 1: return gbitmap_create_with_resource(RESOURCE_ID_COMMON_1);
      case 2: return gbitmap_create_with_resource(RESOURCE_ID_COMMON_2);
      case 3: return gbitmap_create_with_resource(RESOURCE_ID_COMMON_3);
    }
  } else if (treasureID == MAGIC_ID) {
    switch (itemID) {
      case 0: return gbitmap_create_with_resource(RESOURCE_ID_MAGIC_0);
      case 1: return gbitmap_create_with_resource(RESOURCE_ID_MAGIC_1);
      case 2: return gbitmap_create_with_resource(RESOURCE_ID_MAGIC_2);
      case 3: return gbitmap_create_with_resource(RESOURCE_ID_MAGIC_3);
    }
  } else if (treasureID == RARE_ID) {
    switch (itemID) {
      case 0: return gbitmap_create_with_resource(RESOURCE_ID_RARE_0);
      case 1: return gbitmap_create_with_resource(RESOURCE_ID_RARE_1);
      case 2: return gbitmap_create_with_resource(RESOURCE_ID_RARE_2);
      case 3: return gbitmap_create_with_resource(RESOURCE_ID_RARE_3);
    }
  } else if (treasureID == EPIC_ID) {
    switch (itemID) {
      case 0: return gbitmap_create_with_resource(RESOURCE_ID_EPIC_0);
      case 1: return gbitmap_create_with_resource(RESOURCE_ID_EPIC_1);
      case 2: return gbitmap_create_with_resource(RESOURCE_ID_EPIC_2);
      case 3: return gbitmap_create_with_resource(RESOURCE_ID_EPIC_3);
    }
  } else { // LEGENDARY_ID
    switch (itemID) {
      case 0: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_0);
      case 1: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_1);
      case 2: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_2);
      case 3: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_3);
      case 4: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_4);
      case 5: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_5);
      case 6: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_6);
      case 7: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_7);
      case 8: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_8);
      case 9: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_9);
      case 10: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_10);
      case 11: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_11);
      case 12: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_12);
      case 13: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_13);
      case 14: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_14);
      case 15: return gbitmap_create_with_resource(RESOURCE_ID_LEGENDARY_15);
    }
  }
  return NULL;
}

//////////////////////////

void initPrestigeWindowRes() {
  for (uint8_t i=0; i < MAX_UNIQUE; ++i) {
    s_legendaryItem[i] = loadItemImage(LEGENDARY_ID, i);
  }
}

void deinitPrestigeWindowRes() {
  for (uint8_t i=0; i < MAX_UNIQUE; ++i) {
    gbitmap_destroy( s_legendaryItem[i] );
  }
}

GBitmap* getPrestigeItemImage(uint8_t itemID) { return s_legendaryItem[itemID]; }

//GBitmap* getAchievementImage(uint8_t achievementID); // Also used here

//////////////////////////

void initRefineryWindowRes() {
  s_refineryImage[0] = NULL;// TODO
  s_refineryImage[1] = NULL;// TODO
  s_refineryImage[2] = NULL;// TODO
  s_refineryImage[3] = NULL;// TODO
  s_refineryImage[4] = NULL;// TODO
  s_refineryImage[5] = NULL;// TODO
  s_refineryImage[6] = NULL;// TODO
  s_refineryImage[7] = NULL;// TODO
  s_refineryImage[8] = NULL;// TODO
  s_refineryImage[9] = NULL;// TODO
  s_refineryImage[10] = NULL;// TODO
  s_refineryImage[11] = NULL;// TODO
  s_refineryImage[12] = NULL;// TODO
  s_refineryImage[13] = NULL;
  s_refineryImage[14] = NULL;
  s_refineryImage[15] = NULL;
}

void deinitRefineryWindowRes() {
  for (uint8_t i=0; i < N_REFINERY_UPGRADES; ++i) {
    gbitmap_destroy( s_refineryImage[i] );
  }
}

GBitmap* getRefineryImage(uint8_t refineryID) { return s_refineryImage[refineryID]; }

//////////////////////////

void initTankWindowRes() {
  s_tankImage[0] = NULL;// TODO
  s_tankImage[1] = NULL;// TODO
  s_tankImage[2] = NULL;// TODO
  s_tankImage[3] = NULL;// TODO
  s_tankImage[4] = NULL;// TODO
  s_tankImage[5] = NULL;// TODO
  s_tankImage[6] = NULL;// TODO
  s_tankImage[7] = NULL;// TODO
  s_tankImage[8] = NULL;// TODO
  s_tankImage[9] = NULL;// TODO
  s_tankImage[10] = NULL;// TODO
  s_tankImage[11] = NULL;// TODO
  s_tankImage[12] = NULL;// TODO
  s_tankImage[13] = NULL;
  s_tankImage[14] = NULL;
  s_tankImage[15] = NULL;
}

void deinitTankWindowRes() {
  for (uint8_t i=0; i < N_TANK_UPGRADES; ++i) {
    gbitmap_destroy( s_tankImage[i] );
  }
}

GBitmap* getTankImage(uint8_t tankID) { return s_tankImage[tankID]; }

//////////////////////////

void initEmployeeWindowRes() {
  s_employeeImage[0] = NULL;// TODO
  s_employeeImage[1] = NULL;// TODO
  s_employeeImage[2] = NULL;// TODO
  s_employeeImage[3] = NULL;// TODO
  s_employeeImage[4] = NULL;// TODO
  s_employeeImage[5] = NULL;// TODO
  s_employeeImage[6] = NULL;// TODO
  s_employeeImage[7] = NULL;// TODO
  s_employeeImage[8] = NULL;// TODO
  s_employeeImage[9] = NULL;// TODO
  s_employeeImage[10] = NULL;// TODO
  s_employeeImage[11] = NULL;
  s_employeeImage[12] = NULL;
  s_employeeImage[13] = NULL;
  s_employeeImage[14] = NULL;
  s_employeeImage[15] = NULL;
}

void deinitEmployeeWindowRes() {
  for (uint8_t i=0; i < N_WATCHER_UPGRADES; ++i) {
    gbitmap_destroy( s_employeeImage[i] );
  }
}

GBitmap* getEmployeeImage(uint8_t employeeID) { return s_employeeImage[employeeID]; }

//////////////////////////
