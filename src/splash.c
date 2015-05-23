#include "splash.h"
#include "persistence.h"
#include "timeStore.h"
#include "mainWindow.h"
#include "items.h"
#include "timeStore.h"
#include "resources.h"
#include "palette.h"
#include "clock.h"

struct catchUpSummary* catchup;

static Window* s_splashWindow;
static Layer* s_splashLayer;
static AppTimer* s_splashTimer;
char* s_splashText[ITEM_CATEGORIES + 2];
static GColor s_splashTextCol[ITEM_CATEGORIES + 2];
int8_t s_splashTextLines = -1;

void splashClick(ClickRecognizerRef recognizer, void *context) {
  finishSplash();
}

void splashClickConfProvider(Window* window) {
  window_single_click_subscribe(BUTTON_ID_DOWN, splashClick);
  window_single_click_subscribe(BUTTON_ID_SELECT, splashClick);
  window_single_click_subscribe(BUTTON_ID_UP, splashClick);
}

static void splashUpdateProc(Layer *this_layer, GContext *ctx) {
  graphics_draw_text(ctx, "While you\nwere away", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(0, 0, WIN_SIZE_X, 40), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  GRect loc = GRect(5, 55, WIN_SIZE_X-5, 15);
  for (uint8_t i = 0; i <= s_splashTextLines; ++i) {
    //draw3DText(ctx, loc, getGothic14BoldFont(), s_splashText[i], 1, true, GColorWhite, s_splashTextCol[i]);
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
  free(catchup);
  catchup = NULL;
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
  // Decide if we need to show the splash or can go straight to the main win
  if (catchup->timeDiff < (time_t)SEC_IN_MIN) {
    finishSplash(NULL);
    return;
  }
  // Else we show the splash
  s_splashWindow = window_create();
  window_set_window_handlers(s_splashWindow, (WindowHandlers) {
    .load = splashWindowLoad,
    .unload = splashWindowUnload
  });

  // Set the text
  for (int8_t i = -1; i < ITEM_CATEGORIES + 1; ++i) {
    if (i == -1) { // DO TIME
      newLine();
      timeToString(catchup->gainedTime, getTempBuffer(), TEXT_BUFFER_SIZE, true);
      strcpy(s_splashText[s_splashTextLines], "EARNED: ");
      strcat(s_splashText[s_splashTextLines], getTempBuffer());
      s_splashTextCol[s_splashTextLines] = GColorWhite;
    } else if (i == ITEM_CATEGORIES) {// DO MISSED
      if (catchup->missedItem == 0) continue;
      newLine();
      snprintf(s_splashText[s_splashTextLines], TEXT_BUFFER_SIZE, "Missed %i items!", catchup->missedItem);
      s_splashTextCol[s_splashTextLines] = GColorWhite;
    } else { // DO ITEM CATEGORY
      if (catchup->gainedItems[i] == 0) continue;
      newLine();
      snprintf(getTempBuffer(), TEXT_BUFFER_SIZE, "%i", catchup->gainedItems[i]);
      strcpy(s_splashText[s_splashTextLines], ITEM_CATEGORIE_NAME[i]);
      strcat(s_splashText[s_splashTextLines], ": ");
      strcat(s_splashText[s_splashTextLines], getTempBuffer());
      s_splashTextCol[s_splashTextLines] = getTrasureColour(i);
    }
  }

  window_set_background_color(s_splashWindow, GColorDarkGray);
  window_set_click_config_provider(s_splashWindow, (ClickConfigProvider) splashClickConfProvider);
  window_stack_push(s_splashWindow, true);
  s_splashTimer = app_timer_register(SPLASH_DISPLAY_TIME, finishSplash, NULL);
}



void giveCatchupItems(TimeUnits units, uint32_t n) {
  for (uint32_t i = 0; i < n; ++i) {
    uint8_t treasureID = getItemRarity(units);
    uint8_t itemID = 0;
    genRandomItem(&treasureID, &itemID); // Both are modified (rarity may need to be downgraded)
    addItem(treasureID, itemID, 1);
    catchup->gainedItems[treasureID]++;
  }
}

/**
 * Simulate the time collection and item collection functionality while the user is away
 **/
void doCatchup() {
  catchup = malloc(sizeof(struct catchUpSummary));
  memset(catchup, 0, sizeof(struct catchUpSummary));

  if (getUserTimeOfSave() == 0) return; // First load

  catchup->timeDiff = time(NULL) - getUserTimeOfSave();
  // DEBUG
  if (IS_DEBUG) catchup->timeDiff = rand() % SEC_IN_YEAR;
  APP_LOG(APP_LOG_LEVEL_INFO, "Catchup BGN %i seconds", (int)catchup->timeDiff);
  if (catchup->timeDiff < 0) return;

  // Give liquid time. Won't give more than can fit
  uint32_t nMin = catchup->timeDiff / SEC_IN_MIN;
  catchup->gainedTime = safeMultiply( getTimePerMin(), nMin);
  addTime( catchup->gainedTime );
  updateDisplayTime( getUserTime() );

  // Items
  uint32_t nYear =  (catchup->timeDiff / SEC_IN_YEAR);
  uint32_t nMonth = (catchup->timeDiff / (SEC_IN_DAY*31)) - nYear;
  uint32_t nDay =   (catchup->timeDiff / SEC_IN_DAY) - nMonth - nYear;
  uint32_t nHour =  (catchup->timeDiff / SEC_IN_HOUR) - nDay - nMonth - nYear;
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
    for (uint32_t t = 0; t < tries; ++t) {
      if (checkForItem(unit) == false) continue;
      if (getItemAutoCollect() == false) {
        catchup->missedItem++;
        continue;
      }
      giveCatchupItems(unit, 1);
    }
  }
  // For hour and min, we cheat, and do it probabailisticaly.
  // This rounds down to the nearest items boo-hoo to the user
  // Expected N items
  uint32_t dayItems = (nDay * getItemAppearChance(DAY_UNIT)) / SCALE_FACTOR;
  // Now take into account the auto-collect chance
  uint32_t dayItemsKept = (dayItems * getAutoCollectChance()) / SCALE_FACTOR;
  // Finally give the items
  giveCatchupItems(DAY_UNIT, dayItemsKept);
  // And do the same for mins
  uint32_t minItems = (nMin * getItemAppearChance(MINUTE_UNIT)) / SCALE_FACTOR;
  uint32_t minItemsKept = (minItems * getAutoCollectChance()) / SCALE_FACTOR;
  giveCatchupItems(MINUTE_UNIT, minItemsKept);
  // Done
  catchup->missedItem += (dayItems - dayItemsKept) + (minItems - minItemsKept);

  addItemsMissed(catchup->missedItem);
  APP_LOG(APP_LOG_LEVEL_INFO, "Catchup END");

}
