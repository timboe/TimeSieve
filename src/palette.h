#pragma once


GColor getTextColourC();
GColor getTextColourU();
GColor getTextColourR();
GColor getTextColourD();
GColor getTextColourL();

GColor getLiquidTimeColour();
GColor getLiquidTimeHighlightColour();

GColor getSpoogicalColourA();
GColor getSpoogicalColourB();

GColor getTreasureColor(uint8_t treasureID);
GColor getBGFlashColour(uint8_t colourID);

void colourOverride(int8_t palette);
uint8_t getColour();
