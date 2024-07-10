#if !defined(__VIEW_H__)
#define __VIEW_H__

#include "hardware/structs/rosc.h"
#include "observer.hpp"
#include "pico/stdlib.h"
#include "ssd1306_i2c.h"
#include <string>

class View : public Subscriber {
private:
  struct render_area frame_area = {
    start_col : 0,
    end_col : SSD1306_WIDTH - 1,
    start_page : 0,
    end_page : SSD1306_NUM_PAGES - 1
  };

  uint8_t buf[SSD1306_BUF_LEN];

public:
  View() {
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    SSD1306_init();
  }
  ~View() {}

  void init() {
    // Initialize render area for entire frame (SSD1306_WIDTH pixels by
    // SSD1306_NUM_PAGES pages)

    calc_render_area_buflen(&frame_area);

    // zero the entire display
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);
    SSD1306_send_cmd(
        SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state

    // intro sequence: flash the screen 3 times
    // for (int i = 0; i < 3; i++) {
    // SSD1306_send_cmd(SSD1306_SET_ALL_ON); // Set all pixels on
    // sleep_ms(500);
    // SSD1306_send_cmd(
    // SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
    // sleep_ms(500);
    // }
  }

  void update() override {

  }

  void update_counter(size_t val) {
    char buf_text[20];
    int len = sprintf(buf_text, "counter:%d", val);
    WriteString(buf, 5, 0, buf_text);
    render(buf, &frame_area);
  }
};

#endif // __VIEW_H__
