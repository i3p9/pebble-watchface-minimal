#include <pebble.h>
#include <watchface.h>

static void init();
static void deinit();
int main(void);

int main(void) {
  init();
  app_event_loop();
  deinit();
}

static void init(){
	init_watchface();

	update_time(); // init time
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	battery_state_service_subscribe(battery_handler); //init battery
	battery_handler(battery_state_service_peek());
}

static void deinit(){
	deinit_watchface();
}
