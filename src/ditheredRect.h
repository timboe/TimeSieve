#pragma once
#include <pebble.h>

typedef enum{
	TOP_TO_BOTTOM,
	BOTTOM_TO_TOP,
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT,
	TOP_LEFT_TO_BOTTOM_RIGHT,
	BOTTOM_RIGHT_TO_TOP_LEFT,
	TOP_RIGHT_TO_BOTTOM_LEFT,
	BOTTOM_LEFT_TO_TOP_RIGHT
} GradientDirection;

typedef enum{
	DITHER_0_PERCENT,
	DITHER_10_PERCENT,
	DITHER_20_PERCENT,
	DITHER_25_PERCENT,
	DITHER_30_PERCENT,
	DITHER_40_PERCENT,
	DITHER_50_PERCENT,
	DITHER_60_PERCENT,
	DITHER_70_PERCENT,
	DITHER_75_PERCENT,
	DITHER_80_PERCENT,
	DITHER_90_PERCENT,
	DITHER_100_PERCENT
} DitherPercentage;


//=========================================================================================================================
// CONVERSION
//=========================================================================================================================

DitherPercentage getDitherFromPercentage(int percentage);

int getPercentageFromDither(DitherPercentage dither);

//=========================================================================================================================
// DITHERED RECTS
//=========================================================================================================================

void draw_zero_percent(GContext *ctx, GRect bounds, GColor first);

void draw_ten_percent(GContext *ctx, int x_start, int y_start, int width, int height, GColor first, GColor second);

void draw_twenty_percent(GContext *ctx, int x_start, int y_start, int width, int height, GColor first, GColor second);

void draw_twenty_five_percent(GContext *ctx, int x_start, int y_start, int width, int height, GColor first, GColor second);

void draw_thirty_percent(GContext *ctx, int x_start, int y_start, int width, int height, GColor first, GColor second);

void draw_forty_percent(GContext *ctx, int x_start, int y_start, int width, int height, GColor first, GColor second);

void draw_fifty_percent(GContext *ctx, int x_start, int y_start, int width, int height, GColor first, GColor second);

void draw_dithered_rect(GContext *ctx, GRect bounds, GColor first_color, GColor second_color, DitherPercentage percentage);

//=========================================================================================================================
// RANDOM DITHERED RECTS
//=========================================================================================================================

void init_rand();

void draw_random_dithered_rect(GContext *ctx, GRect bounds, GColor first_color, GColor second_color, DitherPercentage dither);
//draw_dithered_rect(ctx, GRect(x_start+(i*width/110)-1, y_start, 1+width/11, height), first, second, getDitherFromPercentage(i));
void draw_random_gradient_rect(GContext *ctx, GRect bounds, GColor first_color, GColor second_color, GradientDirection gradient);

//=========================================================================================================================
// DITHERED CIRCLES
//=========================================================================================================================

void draw_zero_percent_circle(GContext *ctx, int x_center, int y_center, int radius, GColor first);

void draw_ten_percent_circle(GContext *ctx, int x_center, int y_center, int radius, GColor first, GColor second);

void draw_twenty_percent_circle(GContext *ctx, int x_center, int y_center, int radius, GColor first, GColor second);

void draw_twenty_five_percent_circle(GContext *ctx, int x_center, int y_center, int radius, GColor first, GColor second);

void draw_thirty_percent_circle(GContext *ctx, int x_center, int y_center, int radius, GColor first, GColor second);

void draw_forty_percent_circle(GContext *ctx, int x_center, int y_center, int radius, GColor first, GColor second);

void draw_fifty_percent_circle(GContext *ctx, int x_center, int y_center, int radius, GColor first, GColor second);

void draw_dithered_circle(GContext *ctx, int x_center, int y_center, int radius, GColor first_color, GColor second_color, DitherPercentage percentage);

//=========================================================================================================================
// TRANSITIONS
//=========================================================================================================================

void tick_dither();

void draw_transitioning_rect(GContext *ctx, GRect bounds, GColor first_color, GColor second_color);

void start_transitioning_rect(Layer *layer, int delay);

void stop_transitioning_rect();

//=========================================================================================================================
// GRADIENTS
//=========================================================================================================================

void draw_top_to_bottom(GContext *ctx, int x_start, int y_start, int width, int height, GColor first, GColor second);

void draw_left_to_right(GContext *ctx, int x_start, int y_start, int width, int height, GColor first, GColor second);

void draw_gradient_rect(GContext *ctx, GRect bounds, GColor first_color, GColor second_color, GradientDirection direction);