#pragma once
#include <pebble.h>

void settingsWindowLoad(Window* parentWindow);
void settingsWindowUnload();

void settingsSubWindowLoad(Window* parentWindow);
void settingsSubWindowUnload(Window* parentWindow);

void updateSettingsLayer();
