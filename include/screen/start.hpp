#if !defined(__START_VIEW_H__)
#define __START_VIEW_H__

#include "screen_base.hpp"

class StartView : public Screen {
private:
  uint32_t _start_time;
public:
  StartView(ScreenDriverI *driver, Model *model) : Screen(driver, model) {}
  virtual ~StartView() {}

  void on_start() override {
    write_str_row("Isotope OS", 3, screen_align_t::CENTER);
    _start_time = millis();
  }

  void on_draw() override {
    auto sec = (millis() - _start_time) / 1000;
    char *buf = Screen::sb.str_buf;
    memset(buf, 0, 16);
    sprintf(buf, "Loading %u", uint16_t(sec));
    write_str_row(buf, 6, screen_align_t::CENTER);
  }
};

#endif // __START_VIEW_H__
