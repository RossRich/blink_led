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

    void _init_ssd1306() { SSD1306_init(); }

  public:
    Driver() {
      _hw_init();
      _init_ssd1306();
      memset(_buffer, 0, SSD1306_BUF_LEN);
    }
    ~Driver() override {}

    struct render_area frame_area =
        {start_col : 0, end_col : SSD1306_WIDTH - 1, start_page : 0, end_page : SSD1306_NUM_PAGES - 1};

    void write_str(int16_t x, int16_t y, char *str) override {
      calc_render_area_buflen(&frame_area);
      WriteString(_buffer, x, y, str);
      render(_buffer, &frame_area);
    };
    void write_char(int16_t x, int16_t y, uint8_t ch) override{};
  } * _driver;

  Screen *_first;
  Screen *_visible;
  Model *_model;

public:
  View(Model *model) : _model(model) {
    _driver = new Driver();
    _first = new Screen(_driver, model);
    _visible = _first;
    _visible->on_start();
  }

  ~View() {}

  void update() override { _visible->update(); }
};

#endif // __VIEW_H__
