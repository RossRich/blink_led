#if !defined(__SCREEN_H__)
#define __SCREEN_H__

#include "model.hpp"
#include "ssd1306_i2c.h"

const char *protium_name = "protium";
const char *deuterium_name = "deuterium";

static struct FLY_MODES
{
  const char* poctl = "POCTL";
  const char* offboard = "OFFBD";
} modes_str;

class ScreenDriverI {
public:
  virtual ~ScreenDriverI() {}
  virtual void write_str(int16_t x, int16_t y, char *str, struct render_area &frame) = 0;
  virtual void write_char(int16_t x, int16_t y, uint8_t ch, struct render_area &frame) = 0;
};

class Screen {
private:
  ScreenDriverI *_driver;
  Model *_model;
  struct render_area frame_area = {start_col : 0, end_col : SSD1306_WIDTH - 1, start_page : 0, end_page : SSD1306_NUM_PAGES - 1};

public:
  Screen(ScreenDriverI *driver, Model *model) : _driver(driver), _model(model) {}
  ~Screen() {}

  void on_start() {
    frame_area.start_col = 0;
    frame_area.end_col = 7*8;
    frame_area.start_page = 4;
    frame_area.end_page = 5;
    _driver->write_str(frame_area.start_page, frame_area.start_col, "OnStart", frame_area);
  }

  void update() {
    // char buf_text[20];
    // int len = sprintf(buf_text, "counter:%d", _model->get_counter());
    // _driver->write_str(5, 0, buf_text);
  }
};

#endif // __SCREEN_H__
