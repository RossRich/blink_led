#include "include/uav_monitor.hpp"
#include "libs/RF24/RF24.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>


volatile bool timer_fired = false;

Monitor monitor;

bool setup() { return monitor.init(); }

int main() {
  stdio_init_all();
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  while (!setup()) {
    gpio_put(LED_PIN, 0);
    printf("Init failed");
    sleep_ms(500);
    gpio_put(LED_PIN, 1);
  }
  gpio_put(LED_PIN, 0);

  while (true) {
    tight_loop_contents();
  }
}