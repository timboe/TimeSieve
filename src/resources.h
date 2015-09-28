#pragma once
#include <pebble.h>

char* getTempBuffer();

void memRep(void* data);

void drawBitmap(GContext* ctx, const GBitmap* bitmap, GRect rect);

//////////////////////////

void initGlobalRes();
void destroyGlobalRes();

GFont* getGothic14BoldFont();
GBitmap* getQImage();

//////////////////////////

void initMainWindowRes();
void deinitMainWindowRes();
void loadClockFont();

GFont* getDOSFont();
GFont* getClockFont();
GFont* getClockSmallFont();
GFont* getWeatherFont();
GFont* getTemperatureFont();

GBitmap* loadItemImage(const uint8_t treasureID, const uint8_t itemID); // Internal
GBitmap* getSingleItemImage(uint8_t treasureID, uint8_t itemID);
GBitmap* getGemImage(uint8_t treasureID);
GBitmap* getBluetoothImage();

//////////////////////////

void initSplashWindowRes();
void deinitSplashWindowRes();

GFont* getGothic14Font();
GFont* getGothic24BoldFont();

//////////////////////////

void initSellWindowRes();
void deinitSellWindowRes();

GBitmap* getSellItemImage(uint8_t treasureID, uint8_t itemID);

//////////////////////////

void initPrestigeWindowRes();
void deinitPrestigeWindowRes();

GBitmap* getPrestigeItemImage(uint8_t itemID);

//////////////////////////

void initBuyWindowRes(const uint32_t typeID);
void deinitBuyWindowRes(const uint32_t typeID);

GBitmap* getBuyImage(const uint32_t typeID, uint32_t resourceID);

//////////////////////////
