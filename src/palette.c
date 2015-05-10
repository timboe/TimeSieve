#include <pebble.h>
#include "persistence.h"
#include "constants.h"

GColor getTextColourC() {
  switch (getUserSetting(SETTING_COLOUR)) {
  	case PALETTE_BLUE: return GColorLiberty; // Darkest
  	case PALETTE_GREEN: return GColorKellyGreen;
  	case PALETTE_YELLOW: return GColorBrass;
  	case PALETTE_RED: return GColorRed;
  	default: return GColorWhite;
  }
}

GColor getTextColourU() {
  switch (getUserSetting(SETTING_COLOUR)) {
  	case PALETTE_BLUE: return GColorCeleste; // Light U
   	case PALETTE_GREEN: return GColorInchworm;
  	case PALETTE_YELLOW: return GColorPastelYellow;
  	case PALETTE_RED: return GColorSunsetOrange;
  	default: return GColorWhite;
  }
}

GColor getTextColourR() {
  switch (getUserSetting(SETTING_COLOUR)) {
  	case PALETTE_BLUE: return GColorElectricBlue; // Light R
   	case PALETTE_GREEN: return GColorMintGreen;
  	case PALETTE_YELLOW: return GColorIcterine;
  	case PALETTE_RED: return GColorMelon;
  	default: return GColorWhite;
  }
}

GColor getTextColourD() {
  switch (getUserSetting(SETTING_COLOUR)) {
  	case PALETTE_BLUE: return GColorVividCerulean;  // Medium D
   	case PALETTE_GREEN: return GColorMayGreen;
  	case PALETTE_YELLOW: return GColorBrass;
  	case PALETTE_RED: return GColorRoseVale;
  	default: return GColorWhite;
  }
}

GColor getTextColourL() {
  switch (getUserSetting(SETTING_COLOUR)) {
  	case PALETTE_BLUE: return GColorCadetBlue; // Muddy (L)
   	case PALETTE_GREEN: return GColorArmyGreen;
  	case PALETTE_YELLOW: return GColorLimerick;
  	case PALETTE_RED: return GColorBulgarianRose;
  	default: return GColorWhite;
  }
}

GColor getLiquidTimeColour() {
  switch (getUserSetting(SETTING_COLOUR)) {
  	case PALETTE_BLUE: return GColorBlueMoon;
   	case PALETTE_GREEN: return GColorKellyGreen;
  	case PALETTE_YELLOW: return GColorBrass;
  	case PALETTE_RED: return GColorOrange;
  	default: return GColorWhite;
  }
}

GColor getLiquidTimeHighlightColour() {
  switch (getUserSetting(SETTING_COLOUR)) {
  	case PALETTE_BLUE: return GColorCyan;
   	case PALETTE_GREEN: return GColorIcterine;
  	case PALETTE_YELLOW: return GColorYellow;
  	case PALETTE_RED: return GColorRichBrilliantLavender;
  	default: return GColorWhite;
  }	
}

GColor getSpoogicalColourA() {
  switch (getUserSetting(SETTING_COLOUR)) {
  	case PALETTE_BLUE: return GColorPictonBlue;
   	case PALETTE_GREEN: return GColorMediumAquamarine;
  	case PALETTE_YELLOW: return GColorIcterine;
  	case PALETTE_RED: return GColorRajah;
  	default: return GColorWhite;
  }
}

GColor getSpoogicalColourB() {
  switch (getUserSetting(SETTING_COLOUR)) {
  	case PALETTE_BLUE: return GColorMintGreen;
   	case PALETTE_GREEN: return GColorPastelYellow;
  	case PALETTE_YELLOW: return GColorChromeYellow;
  	case PALETTE_RED: return GColorJazzberryJam;
  	default: return GColorWhite;
  }
}

GColor getTrasureColour(uint8_t treasureID) {
  switch (treasureID) {
    case COMMON_ID: return COLOUR_COMMON;
    case MAGIC_ID: return COLOUR_MAGIC;
    case RARE_ID: return COLOUR_RARE;
    case EPIC_ID: return COLOUR_EPIC;
    case LEGENDARY_ID: return COLOUR_LEGENDARY;
    default: return GColorWhite;
  }
}
