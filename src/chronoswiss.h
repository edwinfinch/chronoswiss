#pragma once
#define AMOUNT_OF_HANDS 3

typedef enum HandType {
	HAND_TYPE_SECOND,
	HAND_TYPE_HOUR,
	HAND_TYPE_MINUTE
} HandType;

void chro_tick_handler(struct tm *t, TimeUnits unit);
Window *chro_window_get_window();
void chro_init();
void chro_deinit();