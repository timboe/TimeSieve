#include <pebble.h>
#include "resources.h"
#include "persistence.h"
#include "constants.h"

static GFont s_perfectDOSFont;
static bool s_clockLoaded = false;
static GFont s_clock;
static GFont s_clockSmall;
static GFont s_weatherFont;
static GFont s_g14;
static GFont s_g24b;
static GBitmap* s_QImage = NULL;
static GBitmap* s_singleItemImage = NULL;
static GBitmap* s_gem[ITEM_CATEGORIES] = {NULL};
static GBitmap* s_bluetoothImage = NULL;

static GBitmap* s_item[SELLABLE_CATEGORIES][MAX_TREASURES] = {{NULL},{NULL}};
static GBitmap* s_legendaryItem[MAX_UNIQUE] = {NULL};

static GBitmap* s_resourceImage[UPGRADE_CATEGORIES][MAX_UPGRADES] = {{NULL},{NULL}};

//////////////////////////

void memRep(void* data) {
  APP_LOG(APP_LOG_LEVEL_INFO,"used:%i free:%i",heap_bytes_used(), heap_bytes_free());
  if (data) app_timer_register(1000, memRep, data);
}

//////////////////////////

static char tempBuffer[TEXT_BUFFER_SIZE];

/**
 * For any time we need some extra room for string manipulation
 **/
char* getTempBuffer() {
  return tempBuffer;
}

void initGlobalRes() {
  s_g14  = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_g24b = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_QImage  = gbitmap_create_with_resource(RESOURCE_ID_QMARK);
}

void destroyGlobalRes() {
  gbitmap_destroy( s_QImage );
}

GFont* getGothic14Font() { return &s_g14; }

GFont* getGothic24BoldFont() { return &s_g24b; }

GBitmap* getQImage() { return s_QImage; }

//////////////////////////

void initMainWindowRes() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "initMainWindowRes"); memRep(NULL);
  s_perfectDOSFont  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PERFECT_DOS_21));
  s_weatherFont     = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_WEATHER_21));
  loadClockFont();
  s_gem[COMMON_ID]    = gbitmap_create_with_resource(RESOURCE_ID_GEM_COMMON);
  s_gem[MAGIC_ID]     = gbitmap_create_with_resource(RESOURCE_ID_GEM_MAGIC);
  s_gem[RARE_ID]      = gbitmap_create_with_resource(RESOURCE_ID_GEM_RARE);
  s_gem[EPIC_ID]      = gbitmap_create_with_resource(RESOURCE_ID_GEM_EPIC);
  s_gem[LEGENDARY_ID] = gbitmap_create_with_resource(RESOURCE_ID_GEM_LEGENDARY);
  s_bluetoothImage    = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH);
}

void deinitMainWindowRes() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "DEinitMainWindowRes"); memRep(NULL);
  fonts_unload_custom_font(s_perfectDOSFont);
  fonts_unload_custom_font(s_clock);
  fonts_unload_custom_font(s_clockSmall);
  fonts_unload_custom_font(s_weatherFont);
  s_clockLoaded = false;
  gbitmap_destroy( s_singleItemImage );
  s_singleItemImage = NULL;
  gbitmap_destroy( s_bluetoothImage );
  s_bluetoothImage = NULL;
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

GFont* getTemperatureFont() { return &s_g14; }

GBitmap* getBluetoothImage() { return s_bluetoothImage; }

GBitmap* getSingleItemImage(uint8_t treasureID, uint8_t itemID) {
  gbitmap_destroy( s_singleItemImage );
  s_singleItemImage = loadItemImage(treasureID, itemID);
  return s_singleItemImage;
}

GBitmap* getGemImage(uint8_t treasureID) { return s_gem[treasureID]; }

//////////////////////////

void initSellWindowRes() {
  for (uint8_t i=0; i < SELLABLE_CATEGORIES; ++i) {
    for (uint8_t j=0; j < MAX_TREASURES; ++j) {
      s_item[i][j] = loadItemImage(i, j);
    }
  }
}

void deinitSellWindowRes() {
  for (uint8_t i=0; i < SELLABLE_CATEGORIES; ++i) {
    for (uint8_t j=0; j < MAX_TREASURES; ++j) {
      gbitmap_destroy( s_item[i][j] );
      s_item[i][j] = NULL;
    }
  }
}

GBitmap* getSellItemImage(uint8_t treasureID, uint8_t itemID) {
  return s_item[treasureID][itemID];
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "initPresWindowRes"); memRep(NULL);
  for (uint8_t i=0; i < MAX_UNIQUE; ++i) {
   s_legendaryItem[i] = loadItemImage(LEGENDARY_ID, i); //TODO crash is related to these
  }
}

void deinitPrestigeWindowRes() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "DEinitPresWindowRes"); memRep(NULL);
  for (uint8_t i=0; i < MAX_UNIQUE; ++i) {
    gbitmap_destroy( s_legendaryItem[i] ); //TODO crash is related to these
    s_legendaryItem[i] = NULL;
  }
}

GBitmap* getPrestigeItemImage(uint8_t itemID) { return s_legendaryItem[itemID]; }

//////////////////////////

void initBuyWindowRes(const uint32_t typeID) {
  if (typeID == REFINERY_ID) {
    s_resourceImage[REFINERY_ID][0] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_0);
    s_resourceImage[REFINERY_ID][1] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_1);
    s_resourceImage[REFINERY_ID][2] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_2);
    s_resourceImage[REFINERY_ID][3] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_3);
    s_resourceImage[REFINERY_ID][4] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_4);
    s_resourceImage[REFINERY_ID][5] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_5);
    s_resourceImage[REFINERY_ID][6] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_6);
    s_resourceImage[REFINERY_ID][7] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_7);
    s_resourceImage[REFINERY_ID][8] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_8);
    s_resourceImage[REFINERY_ID][9] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_9);
    s_resourceImage[REFINERY_ID][10] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_10);
    s_resourceImage[REFINERY_ID][11] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_11);
    //s_resourceImage[REFINERY_ID][12] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_12);
    // s_resourceImage[REFINERY_ID][13] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_13;
    // s_resourceImage[REFINERY_ID][14] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_14);
    // s_resourceImage[REFINERY_ID][15] = gbitmap_create_with_resource(RESOURCE_ID_REFINERY_15);
  } else if (typeID == TANK_ID) {
    s_resourceImage[TANK_ID][0] = gbitmap_create_with_resource(RESOURCE_ID_TANK_0);
    s_resourceImage[TANK_ID][1] = gbitmap_create_with_resource(RESOURCE_ID_TANK_1);
    s_resourceImage[TANK_ID][2] = gbitmap_create_with_resource(RESOURCE_ID_TANK_2);
    s_resourceImage[TANK_ID][3] = gbitmap_create_with_resource(RESOURCE_ID_TANK_3);
    s_resourceImage[TANK_ID][4] = gbitmap_create_with_resource(RESOURCE_ID_TANK_4);
    s_resourceImage[TANK_ID][5] = gbitmap_create_with_resource(RESOURCE_ID_TANK_5);
    s_resourceImage[TANK_ID][6] = gbitmap_create_with_resource(RESOURCE_ID_TANK_6);
    s_resourceImage[TANK_ID][7] = gbitmap_create_with_resource(RESOURCE_ID_TANK_7);
    s_resourceImage[TANK_ID][8] = gbitmap_create_with_resource(RESOURCE_ID_TANK_8);
    s_resourceImage[TANK_ID][9] = gbitmap_create_with_resource(RESOURCE_ID_TANK_9);
    s_resourceImage[TANK_ID][10] = gbitmap_create_with_resource(RESOURCE_ID_TANK_10);
    s_resourceImage[TANK_ID][11] = gbitmap_create_with_resource(RESOURCE_ID_TANK_11);
    //s_resourceImage[TANK_ID][12] = gbitmap_create_with_resource(RESOURCE_ID_TANK_12);
    // s_resourceImage[TANK_ID][13] = gbitmap_create_with_resource(RESOURCE_ID_TANK_13);
    // s_resourceImage[TANK_ID][14] = gbitmap_create_with_resource(RESOURCE_ID_TANK_14);
    // s_resourceImage[TANK_ID][15] = gbitmap_create_with_resource(RESOURCE_ID_TANK_15);
  } else { // WATCHER_ID
    s_resourceImage[WATCHER_ID][0] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_0);
    s_resourceImage[WATCHER_ID][1] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_1);
    s_resourceImage[WATCHER_ID][2] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_2);
    s_resourceImage[WATCHER_ID][3] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_3);
    s_resourceImage[WATCHER_ID][4] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_4);
    s_resourceImage[WATCHER_ID][5] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_5);
    s_resourceImage[WATCHER_ID][6] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_6);
    s_resourceImage[WATCHER_ID][7] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_7);
    s_resourceImage[WATCHER_ID][8] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_8);
    s_resourceImage[WATCHER_ID][9] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_9);
    s_resourceImage[WATCHER_ID][10] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_10);
    // s_resourceImage[WATCHER_ID][11] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_11);
    // s_resourceImage[WATCHER_ID][12] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_12);
    // s_resourceImage[WATCHER_ID][13] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_13);
    // s_resourceImage[WATCHER_ID][14] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_14);
    // s_resourceImage[WATCHER_ID][15] = gbitmap_create_with_resource(RESOURCE_ID_EMPLOYEE_15);
  }
}

void deinitBuyWindowRes(const uint32_t typeID) {
  for (uint8_t i=0; i < MAX_UPGRADES; ++i) {
    gbitmap_destroy( s_resourceImage[typeID][i] );
    s_resourceImage[typeID][i] = NULL;
  }
}

GBitmap* getBuyImage(const uint32_t typeID, const uint32_t resourceID) {
  return s_resourceImage[typeID][resourceID];
}
