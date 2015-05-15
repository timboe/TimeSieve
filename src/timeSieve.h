#pragma once
#include <pebble.h>


void create_timeSieve_layer(Window* parentWindow);
void destroy_timeSieve_layer();

void sieveAnimReset(TimeUnits units_changed);
bool sieveAnimCallback(TimeUnits units_changed);

void displyItem(uint8_t treasureID, uint8_t itemID);
void itemCanBeCollected();
bool collectItem(bool autoCollect);
void stopDisplayItem(void* data);

void showNotifyAchievement(uint8_t notifyAchievementID);
void showNotifyTreasure(uint8_t treasureID, uint8_t itemID);
bool stopNotify();
