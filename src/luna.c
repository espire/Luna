#include "pebble.h"
#include "libluna.h"

static Window *window;

static Layer *moon_layer;
static TextLayer *time_layer;

#define NUM_CIRCLE_POINTS 128
#define CIRCLE_RADIUS 64

static GPoint circle_points[NUM_CIRCLE_POINTS];
static GPathInfo circle_path_points = {
  NUM_CIRCLE_POINTS,
  circle_points
};
static GPath *moon_path = NULL;

static void draw_circle_right(float percent) {
  for (int32_t i = 0; i < NUM_CIRCLE_POINTS / 2; ++i) {
    int32_t angle = TRIG_MAX_ANGLE * i / NUM_CIRCLE_POINTS;
    circle_path_points.points[i].x =
        (sin_lookup(angle) * CIRCLE_RADIUS / TRIG_MAX_RATIO * percent);
    circle_path_points.points[i].y =
        (cos_lookup(angle) * CIRCLE_RADIUS / TRIG_MAX_RATIO);
  }
}

static void draw_circle_left(float percent) {
  for (int32_t i = NUM_CIRCLE_POINTS / 2; i < NUM_CIRCLE_POINTS; ++i) {
    int32_t angle = TRIG_MAX_ANGLE * i / NUM_CIRCLE_POINTS;
    circle_path_points.points[i].x =
        (sin_lookup(angle) * CIRCLE_RADIUS / TRIG_MAX_RATIO * percent);
    circle_path_points.points[i].y =
        (cos_lookup(angle) * CIRCLE_RADIUS / TRIG_MAX_RATIO);
  }
}

static void draw_circles(double moon_percent) {
  // The new moon begins from the right and fills to the left.
  // Then, it empties from right to left.
  if (moon_percent < 0.5) {
    draw_circle_right(1.0);
    draw_circle_left(moon_percent * 4 - 1);
  } else if (moon_percent > 0.5) {
    draw_circle_left(1.0);
    draw_circle_right(1 - (moon_percent - 0.5) * 4);
  } else if (moon_percent == 0.5) {
    draw_circle_right(1.0);
    draw_circle_left(1.0);
  }
}

static void moon_layer_update_callback(Layer *me, GContext *ctx) {
  (void)me;

  time_t now = time(NULL);
  struct tm *t  = localtime(&now);
  double moon_age = moon_age_from_time(t->tm_year + 1900, t->tm_mon + 1,
                                       t->tm_mday, t->tm_hour);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Today's date: %d/%d/%d:%d", t->tm_year + 1900,
          t->tm_mon + 1, t->tm_mday, t->tm_hour);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Moon age: %d", (int)moon_age);

  double moon_percent = moon_age / SYNODIC_MONTH;

  draw_circles(moon_percent);

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, moon_path);
}

static void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(moon_layer);
}

static void update_time() {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  static char* buffer = "00:00 Mon 00";
  if(clock_is_24h_style()) {
    strftime(buffer, sizeof("00:00 Mon 00"), "%H:%M %b %d", t);
  } else {
    strftime(buffer, sizeof("00:00 Mon 00"), "%I:%M %b %d", t);
  }
  text_layer_set_text(time_layer, buffer);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window* window) {
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  moon_layer = layer_create(bounds);
  layer_set_update_proc(moon_layer, moon_layer_update_callback);
  layer_add_child(window_layer, moon_layer);

  // Create the moon's drawing path and move it to the center of the screen
  moon_path = gpath_create(&circle_path_points);
  gpath_move_to(moon_path, GPoint(bounds.size.w/2, bounds.size.h/2 - 12));

  time_layer = text_layer_create(GRect(0, 136, 144, 32));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_font(time_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(time_layer));
  
  update_time();
}

static void main_window_unload(Window* window) {
  gpath_destroy(moon_path);
  layer_destroy(moon_layer);
  text_layer_destroy(time_layer);
}

static void init() {
  window = window_create();

  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(window, true /* Animated */);

  tick_timer_service_subscribe(HOUR_UNIT, handle_hour_tick);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit() {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
