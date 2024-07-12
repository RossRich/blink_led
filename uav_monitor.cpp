#include "controller.hpp"
#include "libs/RF24/RF24.h"
#include "model.hpp"
#include "pico/stdlib.h"
#include "radio.hpp"
#include "tusb.h"
#include "view.hpp"
#include <stdio.h>

View *_view;
Model *_model;
Controller *_controller;

bool setup() {
  _model = new Model();
  _view = new View(_model);
  _controller = new Controller(_model, _view);
  return _controller->init();
}

int main() {
  stdio_init_all();
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
    tight_loop_contents();
  }
}