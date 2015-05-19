#pragma once

void init_mainWindow();
void destroy_mainWindow();

void main_window_load(Window *window);
void main_window_unload(Window *window);

void animBegin();

void update_tick_handler();

TimeUnits getLastTimeUnit();
