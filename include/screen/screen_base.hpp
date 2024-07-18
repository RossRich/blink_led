#if !defined(__SCREEN_BASE_H__)
#define __SCREEN_BASE_H__

#include "../model.hpp"
#include "ssd1306_i2c.h"
#include <ctype.h>

#define SSD_LAST_PAGE (SSD1306_NUM_PAGES - 1u)
#define SSS_LAST_COL (SSD1306_WIDTH - 1u)
#define SCREEN_FPS 15u

enum screen_align_t {
  TOP = uint8_t(1),
  BOTTOM = uint8_t(1 << 1),
  LEFT = uint8_t(1 << 2),
  RIGHT = uint8_t(1 << 3),
  CENTER = uint8_t(1 << 4)
};

class ScreenDriverI {
public:
  virtual ~ScreenDriverI() {}
  virtual void write_str(int16_t x, int16_t y, const char *str,
                         struct render_area &frame) = 0;
  virtual void write_char(int16_t x, int16_t y, const uint8_t ch,
                          struct render_area &frame) = 0;
};

struct ScreenBase {
  char str_buf[SSD1306_WIDTH / SSD1306_PAGE_HEIGHT];
};

class Screen {
private:
  struct render_area frame_area = {
    start_col : 0,
    end_col : SSD1306_WIDTH - 1,
    start_page : 0,
    end_page : SSD1306_NUM_PAGES - 1
  };

protected:
  absolute_time_t _update_timer = get_absolute_time();
  static ScreenBase sb;
  ScreenDriverI *_driver;
  Model *_model;
  uint16_t _update_rate_ms = uint16_t((1.0f / SCREEN_FPS) * 1000);
  uint32_t timex = millis();

public:
  Screen(ScreenDriverI *driver, Model *model)
      : _driver(driver), _model(model) {}
  ~Screen() {}

  void write_str(const char *str, uint8_t x, uint8_t y) {
    render_area ra;
    _driver->write_str(x, y, str, ra);
  }

  void write_str_row(const char *str, uint8_t row, uint8_t align,
                     bool is_preclean = false) {
    uint8_t x = 0;
    uint8_t y = row * SSD1306_PAGE_HEIGHT;

    auto len = strlen(str);

    if (align & uint8_t(screen_align_t::CENTER)) {
      uint8_t m = len % 2 ? uint8_t(len / 2) + 1 : uint8_t(len / 2);
      x = 8u - m;
    }

    write_str(str, x * SSD1306_PAGE_HEIGHT, y);
  }

  void draw() {
    if (not time_reached(_update_timer))
      return;

    _update_timer = make_timeout_time_ms(_update_rate_ms);
    // printf("draw: %lu\n", millis() - timex);
    timex = millis();

    on_draw();
  }

  virtual void on_start() {}

  virtual void on_draw() = 0;
};

ScreenBase Screen::sb;

#endif // __SCREEN_BASE_H__
