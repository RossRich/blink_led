#include "controller.hpp"
#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>

int main() {
  stdio_init_all();

  i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

  gpio_init(LED_PIN);
  gpio_init(ENC_SW);
  gpio_init(ENC_CLK);
  gpio_init(ENC_DT);

  gpio_set_dir(ENC_SW, GPIO_IN);
  gpio_set_dir(ENC_DT, GPIO_IN);
  gpio_set_dir(ENC_CLK, GPIO_IN);
  gpio_pull_up(ENC_SW);

  gpio_set_dir(LED_PIN, GPIO_OUT);

  Controller controller;
  while (!controller.init()) {
    printf("Init failed");
    gpio_put(LED_PIN, 0);
    sleep_ms(500);
    gpio_put(LED_PIN, 1);
    sleep_ms(500);
  }
  gpio_put(LED_PIN, 0);

  while (true) {
    controller.run();
  }
}