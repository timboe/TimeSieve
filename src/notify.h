#pragma once
#include <pebble.h>

void removeNotify(void* data);
void showNotify(GColor highlight, const char* a, const  char* b, const char* c);

Layer* getNotifyLayer();
void destroyNotifyLayer();
