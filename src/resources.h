#pragma once
#include <pebble.h>

char* getTempBuffer();

//////////////////////////

void initMainWindowRes();
void deinitMainWindowRes();
void loadClockFont();

GFont* getDOSFont();
GFont* getClockFont();
GFont* getClockSmallFont();
GFont* getWeatherFont();
GFont* getTemperatureFont();
GFont* getGothic14BoldFont();


GBitmap* loadItemImage(const uint8_t treasureID, const uint8_t itemID); // Internal
GBitmap* getSingleItemImage(uint8_t treasureID, uint8_t itemID);
GBitmap* getGemImage(uint8_t treasureID);

void init_resources();
void destroy_resources();

//////////////////////////

void initSellWindowRes();
void deinitSellWindowRes();

GBitmap* getSellItemImage(uint8_t treasureID, uint8_t itemID);

//////////////////////////

void initPrestigeWindowRes();
void deinitPrestigeWindowRes();

GBitmap* getPrestigeItemImage(uint8_t itemID);

//////////////////////////

void initRefineryWindowRes();
void deinitRefineryWindowRes();

GBitmap* getRefineryImage(uint8_t refineryID);

//////////////////////////

void initTankWindowRes();
void deinitTankWindowRes();

GBitmap* getTankImage(uint8_t tankID);

//////////////////////////

void initEmployeeWindowRes();
void deinitEmployeeWindowRes();

GBitmap* getEmployeeImage(uint8_t employeeID);

//////////////////////////
