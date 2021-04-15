#include <pebble.h>
#include "watchface.h"

static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static Layer *s_battery_layer;

static int s_battery_level;

void main_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_time_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(65, 59), bounds.size.w, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  s_date_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(41, 35), bounds.size.w, 30));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "Mon, 01 Jan");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

  s_battery_layer = layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(118, 112), bounds.size.w, 10));
  layer_set_update_proc(s_battery_layer, update_battery);
  layer_add_child(window_layer, s_battery_layer);
  APP_LOG(APP_LOG_LEVEL_INFO, "Battery Icon is displayed");
}

void main_window_unload(Window *window){
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);

  layer_destroy(s_battery_layer);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}

void update_time(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
    "%H:%M" : "%I:%M", tick_time);

  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a, %d %b", tick_time);

  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
}

void update_battery(Layer *layer, GContext *ctx){
  double battery_remain; //1: 10%/20%, 2: 30%/40%
  if (s_battery_level % 20 == 10)
    battery_remain = ((int) s_battery_level / 20) + 1;
  else
    battery_remain = (int) s_battery_level  / 20;

  int x = PBL_IF_ROUND_ELSE(57.5, 39.5), y = 4.5;
  graphics_context_set_fill_color(ctx, GColorBlack);

  for (int i = 0; i < 5; ++i)
  {
    if(i < battery_remain)
      graphics_fill_circle(ctx, GPoint(x,y),3);
    else
      graphics_draw_circle(ctx, GPoint(x,y), 3);
    x = x + 15;
  }
}

void battery_handler(BatteryChargeState charge_state){
  s_battery_level = charge_state.charge_percent;
  layer_mark_dirty(s_battery_layer);
}

void init_watchface() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
}

void deinit_watchface() { //destroy window
  window_destroy(s_main_window);
}
