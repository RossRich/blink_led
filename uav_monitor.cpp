#include "controller.hpp"
#include "libs/RF24/RF24.h"
#include "model.hpp"
#include "pico/stdlib.h"
#include "radio.hpp"
#include "tusb.h"
#include "view.hpp"
#include <stdio.h>

View *view;
Model *model;
Controller *controller;

bool setup() {
  model = new Model();
  view = new View(model);
  controller = new Controller(model, view);
  return controller->init();
}

int main() {
  stdio_init_all();

  i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  while (!setup()) {
    printf("Init failed");
    gpio_put(LED_PIN, 0);
    sleep_ms(500);
    gpio_put(LED_PIN, 1);
    sleep_ms(500);
  }
  gpio_put(LED_PIN, 0);

  while (true) {
    controller->run();
    // tight_loop_contents();
  }
}