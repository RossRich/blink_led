#if !defined(__VIEW_H__)
#define __VIEW_H__

#include "hardware/structs/rosc.h"
#include "model.hpp"
#include "observer.hpp"
#include "pico/stdlib.h"
#include "screen.hpp"
#include "ssd1306_i2c.h"

class View : public Subscriber {
private:
  class Driver : public ScreenDriverI {
  private:
    uint8_t _buffer[SSD1306_BUF_LEN];

    void _hw_init() {
      i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
      gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
      gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
      gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
      gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    }

    void _init_ssd1306() {
      SSD1306_init();
      // SSD1306_send_cmd(SSD1306_SET_ALL_ON); // Set all pixels on
      // sleep_ms(250);
      // SSD1306_send_cmd(SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
    }

  public:
    Driver() {
      _hw_init();
      _init_ssd1306();
      memset(_buffer, 0, SSD1306_BUF_LEN);
    }
    ~Driver() override {}

    void clear() {
      struct render_area rd;
      rd.start_col = 0;
      rd.end_col = SSD1306_WIDTH - 1;
      rd.start_page = 0;
      rd.end_page = SSD1306_NUM_PAGES - 1;
      calc_render_area_buflen(&rd);
      memset(_buffer, 0, SSD1306_BUF_LEN);
      render(_buffer, &rd);
    }

    void write_str(int16_t x, int16_t y, char *str, render_area &frame) override {
      WriteString(_buffer, x, y, str);
      calc_render_area_buflen(&frame);
      render(_buffer, &frame);
    };
    void write_char(int16_t x, int16_t y, uint8_t ch, struct render_area &frame) override {};
  } *_driver;

  Screen *_first;
  Screen *_visible;
  Model *_model;

public:
  View(Model *model) : _model(model) {
    _driver = new Driver();
    _first = new Screen(_driver, model);
    _driver->clear();
    _visible = _first;
    _visible->on_start();
  }

  ~View() {}

  void update() override { _visible->update(); }
};

#endif // __VIEW_H__
