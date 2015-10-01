#pragma once

typedef enum {
  WINDOW_SPLASH,
  WINDOW_MAIN,
  WINDOW_SELL,
  WINDOW_BUY,
  WINDOW_REFINARY,
  WINDOW_TANK,
  WINDOW_EMPLOYEE,
  WINDOW_SETTINGS,
  WINDOW_ACHIEVEMENT,
  WINDOW_LEGENDARIES,
} TIMESINK_WINDOWS_TYPE;

void initWindows();
void destroyWindows();

void popWindow(bool anim);
void pushWindow(TIMESINK_WINDOWS_TYPE window, bool anim);
void popPushWindow(TIMESINK_WINDOWS_TYPE window, bool anim);
