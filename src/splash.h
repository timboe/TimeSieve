#pragma once
#include "constants.h"

bool getNeedSplashScreen();

void splashWindowLoad(Window* window);
void splashWindowUnload();

void doCatchup();
void finishSplash();

void splashClickConfProvider(Window* window);
