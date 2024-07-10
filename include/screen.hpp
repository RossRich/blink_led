#if !defined(__SCREEN_H__)
#define __SCREEN_H__

#include "model.hpp"

class ScreenDriverI {
public:
  virtual ~ScreenDriverI() {}
  virtual void write_str(int16_t x, int16_t y, char *str) = 0;
  virtual void write_char(int16_t x, int16_t y, uint8_t ch) = 0;
};

class Screen {
private:
  ScreenDriverI *_driver;
  Model *_model;

public:
  Screen(ScreenDriverI *driver, Model *model) : _driver(driver), _model(model) {}
  ~Screen() {}

  void on_start() {
    printf("On start screen\n");
    // _driver->write_str(5, 0, "OnStart\0");
  }

  void update() {
    // char buf_text[20];
    // int len = sprintf(buf_text, "counter:%d", _model->get_counter());
    // _driver->write_str(5, 0, buf_text);
    printf("update screen\n");
  }
};

#endif // __SCREEN_H__
