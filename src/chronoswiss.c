#include <pebble.h>
#include "chronoswiss.h"

Window *chro_window;
GBitmap *chrono_bitmap;
BitmapLayer *chrono_background;
Layer *chro_main_layer;
GPath *paths[AMOUNT_OF_HANDS];
#ifdef INVERTED
	InverterLayer *theme;
#endif

const struct GPathInfo HAND_PATHS[AMOUNT_OF_HANDS] = {
	/*
	{ //Second hand
  	.num_points = 25,
    .points = (GPoint []) {
      {73, 91},
      {73, 109},
      {74, 109},
      {74, 114},
      {73, 114},
      {73, 115},
      {75, 117},
      {75, 119},
      {73, 121},
      {73, 129},
      {74, 129},
      {74, 133},
      {70, 133},
      {70, 129},
      {71, 129},
      {71, 121},
      {69, 119},
      {69, 117},
      {71, 115},
      {71, 114},
      {70, 144},
      {70, 109},
      {71, 109},
      {71, 91},
      {72, 92}
    }
  },
  { //Minute hand
  	.num_points = 23,
    .points = (GPoint []) {
      {73, 27},
      {73, 38},
      {74, 38},
      {74, 50},
      {73, 50},
      {73, 68},
      {74, 68},
      {74, 79},
      {76, 81},
      {76, 86},
      {74, 88},
      {69, 88},
      {67, 86},
      {67, 81},
      {69, 79},
      {69, 68},
      {70, 68},
      {70, 50},
      {69, 50},
      {69, 38},
      {70, 38},
      {70, 27},
      {72, 27}
    }
  },
  */
  { //Second hand
  	  .num_points = 3,
	  .points = (GPoint[]) {
	    {4, 0},
	    {-4, 0},
	    {4,  -40},
	  }
  },
  { //Hour hand
  	  .num_points = 3,
	  .points = (GPoint[]) {
	    {4, 0},
	    {-4, 0},
	    {4,  -40},
	  }
  },
  { //Minute hand
  	  .num_points = 3,
	  .points = (GPoint[]) {
	    {8, 0},
	    {-8, 0},
	    {8,  -80},
	  }
  }
  /*
  { //Hour hand
    .num_points = 19,
    .points = (GPoint []) {
      {72, 26},
      {72, 28},
      {73, 28},
      {73, 30},
      {76, 33},
      {76, 36},
      {73, 39},
      {73, 48},
      {74, 49},
      {74, 51},
      {71, 52},
      {70, 51},
      {70, 49},
      {71, 48},
      {71, 38},
      {68, 36},
      {68, 33},
      {71, 30},
      {71, 28}
    }
  },
  */
};

void chro_update_proc(Layer *layer, GContext *ctx){
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_context_set_stroke_color(ctx, GColorBlack);
	
	for(int i = 0; i < AMOUNT_OF_HANDS; i++){
		switch(i){
			case HAND_TYPE_SECOND:;
				int angle1 = t->tm_sec * 6;
				gpath_rotate_to(paths[HAND_TYPE_SECOND], (TRIG_MAX_ANGLE / 360) * angle1);
				break;
			case HAND_TYPE_MINUTE:;
				int angle2 = t->tm_min * 6;
				gpath_rotate_to(paths[HAND_TYPE_MINUTE], (TRIG_MAX_ANGLE / 360) * angle2);
				break;
			case HAND_TYPE_HOUR:;
				int angle3 = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
				gpath_rotate_to(paths[HAND_TYPE_HOUR], angle3);
				break;
		}
		gpath_draw_filled(ctx, paths[i]);
		gpath_draw_outline(ctx, paths[i]);
	}
	gpath_move_to(paths[HAND_TYPE_MINUTE], GPoint(72, 84));
	gpath_move_to(paths[HAND_TYPE_HOUR], GPoint(70, 52));
	gpath_move_to(paths[HAND_TYPE_SECOND], GPoint(72, 118));
}

void chro_tick_handler(struct tm *t, TimeUnits unit){
	layer_mark_dirty(chro_main_layer);
}

void chro_window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);

	chrono_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

	chrono_background = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(chrono_background, chrono_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(chrono_background));

	for(int i = 0; i < AMOUNT_OF_HANDS; i++){
		paths[i] = gpath_create(&HAND_PATHS[i]);
	}

	chro_main_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(chro_main_layer, chro_update_proc);
	layer_add_child(window_layer, chro_main_layer);
	
	#ifdef INVERTED
		theme = inverter_layer_create(GRect(0, 0, 144, 168));
		layer_add_child(window_layer, inverter_layer_get_layer(theme));
	#endif
	
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	chro_tick_handler(t, SECOND_UNIT);
	layer_mark_dirty(chro_main_layer);
}

void chro_window_unload(Window *window){
	gbitmap_destroy(chrono_bitmap);
	bitmap_layer_destroy(chrono_background);
	layer_destroy(chro_main_layer);
}

Window *chro_window_get_window(){
	return chro_window;
}

void chro_init(){
	chro_window = window_create();
	window_set_background_color(chro_window, GColorBlack);
	window_set_window_handlers(chro_window, (WindowHandlers){
		.load = chro_window_load,
		.unload = chro_window_unload
	});
	tick_timer_service_subscribe(SECOND_UNIT, chro_tick_handler);
}

void chro_deinit(){
	window_destroy(chro_window);
}