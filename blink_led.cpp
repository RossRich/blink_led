#include "include/uav_monitor.hpp"
#include "libs/RF24/RF24.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>

const uint8_t SYS_ID = 2;
const uint8_t COMP_ID = MAV_COMP_ID_ALL;
const uint8_t chan = MAVLINK_COMM_0;
const uint LED_PIN = PICO_DEFAULT_LED_PIN;
volatile bool timer_fired = false;

uint radio_num = 1;

// mavlink_message_t msg;
// mavlink_heartbeat_t hb;
// mavlink_status_t status;

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

  // mavlink_status_t status;
  // mavlink_message_t msg;
  // int chan = MAVLINK_COMM_0;

  while (true) {
    tight_loop_contents();
  }
}