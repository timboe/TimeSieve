#pragma once
#include <pebble.h>

static GFont s_perfectDOSFont;

GFont* getDOSFont();

void init_resources();
void destroy_resources();