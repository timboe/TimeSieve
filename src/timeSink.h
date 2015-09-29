#pragma once
#include <pebble.h>
#include <constants.h>

void create_timeSink_layer(Window* parentWindow);
void destroy_timeSink_layer();

void sinkAnimReset(TimeUnits units_changed);
bool sinkAnimCallback(TimeUnits units_changed);

void displyItem(uint8_t treasureID, uint8_t itemID);
void itemCanBeCollected();
bool collectItem(bool autoCollect);
void stopDisplayItem(void* data);

void updateWeatherBuffer();
void updateWeather(int16_t temp, weatherType weatherCode);

void showNotifyAchievement(uint8_t notifyAchievementID);
void showNotifyTreasure(uint8_t treasureID, uint8_t itemID);
bool stopNotify();
