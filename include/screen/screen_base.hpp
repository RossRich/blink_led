#if !defined(__SCREEN_BASE_H__)
#define __SCREEN_BASE_H__

#include "../model.hpp"
#include "ssd1306_i2c.h"
#include <ctype.h>

#define SSD_LAST_PAGE (SSD1306_NUM_PAGES - 1u)
#define SSS_LAST_COL (SSD1306_WIDTH - 1u)

class ScreenDriverI {
public:
  virtual ~ScreenDriverI() {}
  virtual void write_str(int16_t x, int16_t y, const char *str,
                         struct render_area &frame) = 0;
  virtual void write_char(int16_t x, int16_t y, const uint8_t ch,
                          struct render_area &frame) = 0;
};

struct ScreenBase {
  char str_buf[16];
};

class Screen {
private:
  ScreenDriverI *_driver;
  Model *_model;

  struct render_area frame_area = {
    start_col : 0,
    end_col : SSD1306_WIDTH - 1,
    start_page : 0,
    end_page : SSD1306_NUM_PAGES - 1
  };

protected:
  static ScreenBase sb;

public:
  Screen(ScreenDriverI *driver, Model *model)
      : _driver(driver), _model(model) {}
  ~Screen() {}

  void write_str(const char *str, uint8_t x, uint8_t y) {
    size_t len = strlen(str);

    render_area ra;
    ra.start_col = x;
    ra.end_col = len * SSD1306_PAGE_HEIGHT + x;
    ra.start_page = y;
    ra.end_page = y;
    _driver->write_str(x, y, str, ra);
  }

  virtual void on_start() {}

  virtual void update() = 0;
};

ScreenBase Screen::sb;

#endif // __SCREEN_BASE_H__
