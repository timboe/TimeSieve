#include "splash.h"
#include "persistence.h"
#include "timeStore.h"
#include "mainWindow.h"
#include "items.h"
#include "timeStore.h"
#include "resources.h"
#include "palette.h"
#include "clock.h"

static Window* s_splashWindow;
static Layer* s_splashLayer;
static AppTimer* s_splashTimer;
char* s_splashText[ITEM_CATEGORIES + 2];
static char s_title[TEXT_BUFFER_SIZE];
static GColor s_splashTextCol[ITEM_CATEGORIES + 2];
int8_t s_splashTextLines = -1;
static bool s_doSplash;

/**
 * We show the splash if the user has gained any time/items
 * or if the iterative price buffer loop has more than, say,
 * 400 iterations.
 **/
bool getNeedSplashScreen() {
  s_doSplash = false;
  if (getUserTimeOfSave() == 0) s_doSplash = false; // First load
  else if ((time(NULL) - getUserTimeOfSave()) >= (time_t)SEC_IN_MIN) s_doSplash = true;
  else if (getUserGrandTotalUpgrades() > 400) s_doSplash = true;
  return s_doSplash;
}

void splashClick(ClickRecognizerRef recognizer, void *context) {
  finishSplash();
}

void splashClickConfProvider(Window* window) {
  window_single_click_subscribe(BUTTON_ID_DOWN, splashClick);
  window_single_click_subscribe(BUTTON_ID_SELECT, splashClick);
  window_single_click_subscribe(BUTTON_ID_UP, splashClick);
}

static void splashUpdateProc(Layer *this_layer, GContext *ctx) {
  draw3DText(ctx, GRect(0, 0, WIN_SIZE_X, 40), getGothic24BoldFont(), s_title, 1, true, GColorWhite, GColorBlack);
  GRect loc = GRect(5, 55, WIN_SIZE_X-5, 15);
  for (uint8_t i = 0; i <= s_splashTextLines; ++i) {
    draw3DText(ctx, loc, getGothic14Font(), s_splashText[i], 1, true, s_splashTextCol[i], GColorBlack);
    loc.origin.y += 15;
  }
}

void splashWindowLoad(Window* window) {
  Layer* windowLayer = window_get_root_layer(window);
  s_splashLayer = layer_create( GRect(0, 0, WIN_SIZE_X, WIN_SIZE_Y) );
  layer_add_child(windowLayer, s_splashLayer);
  layer_set_update_proc(s_splashLayer, splashUpdateProc);
}

void splashWindowUnload() {
  layer_destroy(s_splashLayer);
  s_splashLayer = 0;
}

void finishSplash(void* data) {
  app_timer_cancel(s_splashTimer);
  s_splashTimer = NULL;
  window_stack_remove(s_splashWindow, true);
  window_destroy(s_splashWindow);
  s_splashWindow = NULL;
  for (uint8_t i = 0; i <= s_splashTextLines; ++i) {
    free(s_splashText[i]);
  }
  // Invoke the main window
  init_mainWindow();
}

void newLine() {
  s_splashText[++s_splashTextLines] = malloc(TEXT_BUFFER_SIZE*sizeof(char));
}

void showSplash() {
  // show the splash
  s_splashWindow = window_create();
  window_set_window_handlers(s_splashWindow, (WindowHandlers) {
    .load = splashWindowLoad,
    .unload = splashWindowUnload
  });

  window_set_background_color(s_splashWindow, GColorDarkGray);
  window_stack_push(s_splashWindow, true);
  strcpy(s_title, "Catching Up...");
}

void giveCatchupItems(TimeUnits units, uint32_t n, uint16_t* counterPointer) {
  for (uint32_t i = 0; i < n; ++i) {
    uint8_t treasureID = getItemRarity(units);
    uint8_t itemID = 0;
    genRandomItem(&treasureID, &itemID); // Both are modified (rarity may need to be downgraded)
    addItem(treasureID, itemID, 1);
    (*(counterPointer + treasureID))++;
  }
}

/**
 * Simulate the time collection and item collection functionality while the user is away
 **/
void doCatchup() {
  if (s_doSplash == false) return;

  int32_t timeDiff = time(NULL) - getUserTimeOfSave();
  if (IS_DEBUG) timeDiff = rand() % SEC_IN_YEAR;   // DEBUG

  APP_LOG(APP_LOG_LEVEL_INFO, "Catchup BGN %i seconds", (int)timeDiff);

  uint16_t missedItem = 0;
  uint16_t gainedItems[ITEM_CATEGORIES] = {0};

  // Give liquid time. Won't give more than can fit
  uint32_t nMin = timeDiff / SEC_IN_MIN;
  uint64_t gainedTime = safeMultiply( getTimePerMin(), nMin);
  addTime( gainedTime );
  updateDisplayTime( getUserTime() );

  // Items
  uint32_t nYear =  (timeDiff / SEC_IN_YEAR);
  uint32_t nMonth = (timeDiff / (SEC_IN_DAY*31)) - nYear;
  uint32_t nDay =   (timeDiff / (SEC_IN_DAY/2)) - nMonth - nYear; // Remember we do 'day' on midday AND midnight
  uint32_t nHour =  (timeDiff / SEC_IN_HOUR) - nDay - nMonth - nYear;
  nMin = nMin - nHour - nDay - nMonth - nYear;
  // For year, month and day we do it proper, i.e. we check each boundary in turn
  for (uint8_t N = 0; N < 3; ++N) {
    uint32_t tries = nYear;
    TimeUnits unit = YEAR_UNIT;
    if (N == 1) {
      tries = nMonth;
      unit = MONTH_UNIT;
    } else if (N == 2) {
      tries = nDay;
      unit = DAY_UNIT;
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Catchup #%i checking %i tries", N, (int)tries);
    for (uint32_t t = 0; t < tries; ++t) {
      if (checkForItem(unit) == false) continue;
      if (getItemAutoCollect() == false) {
        ++missedItem;
        continue;
      }
      giveCatchupItems(unit, 1, gainedItems);
    }
  }
  // For hour and min, we cheat, and do it probabailisticaly.
  // This rounds down to the nearest items boo-hoo to the user
  // Expected N items
  uint32_t hourItems = (nHour * getItemAppearChance(HOUR_UNIT)) / SCALE_FACTOR;
  // Now take into account the auto-collect chance
  uint32_t hourItemsKept = (hourItems * getAutoCollectChance()) / SCALE_FACTOR;
  // Finally give the items
  giveCatchupItems(HOUR_UNIT, hourItemsKept, gainedItems);
  // And do the same for mins
  uint32_t minItems = (nMin * getItemAppearChance(MINUTE_UNIT)) / SCALE_FACTOR;
  uint32_t minItemsKept = (minItems * getAutoCollectChance()) / SCALE_FACTOR;
  giveCatchupItems(MINUTE_UNIT, minItemsKept, gainedItems);
  // Done
  missedItem += (hourItems - hourItemsKept) + (minItems - minItemsKept);

  addItemsMissed(missedItem);

  // Popuate gfx
  strcpy(s_title, "While you\nwere away");
  for (int8_t i = -1; i < ITEM_CATEGORIES + 1; ++i) {
    if (i == -1) { // DO TIME
      newLine();
      timeToString(gainedTime, getTempBuffer(), TEXT_BUFFER_SIZE, true);
      strcpy(s_splashText[s_splashTextLines], "EARNED: ");
      strcat(s_splashText[s_splashTextLines], getTempBuffer());
      s_splashTextCol[s_splashTextLines] = GColorWhite;
    } else if (i == ITEM_CATEGORIES) {// DO MISSED
      if (missedItem == 0) continue;
      newLine();
      snprintf(s_splashText[s_splashTextLines], TEXT_BUFFER_SIZE, "MISSED %i items!", missedItem);
      s_splashTextCol[s_splashTextLines] = GColorRed;
    } else { // DO ITEM CATEGORY
      if (gainedItems[i] == 0) continue;
      newLine();
      snprintf(getTempBuffer(), TEXT_BUFFER_SIZE, "%i", gainedItems[i]);
      strcpy(s_splashText[s_splashTextLines], ITEM_CATEGORIE_NAME[i]);
      strcat(s_splashText[s_splashTextLines], ": ");
      strcat(s_splashText[s_splashTextLines], getTempBuffer());
      s_splashTextCol[s_splashTextLines] = getTreasureColor(i);
    }
  }

  s_splashTimer = app_timer_register(SPLASH_DISPLAY_TIME, finishSplash, NULL);
  window_set_click_config_provider(s_splashWindow, (ClickConfigProvider) splashClickConfProvider); // Alow to click thru
  layer_mark_dirty(s_splashLayer);

  APP_LOG(APP_LOG_LEVEL_INFO, "Catchup END");
}
