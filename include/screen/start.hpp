#if !defined(__START_VIEW_H__)
#define __START_VIEW_H__

#include "screen_base.hpp"

class StartView : public Screen {
private:
  uint counter = 0;
public:
  StartView(ScreenDriverI *driver, Model *model) : Screen(driver, model) {}
  virtual ~StartView() {}

  void on_start() override {
    const char *test = "start on start";
    write_str(test, 0, 4);
    printf(test);
  }

  void update() override {
    sprintf(Screen::sb.str_buf, "start: %d  ", counter);
    write_str(sb.str_buf, 0, 4);
    counter += 5;
  }
};

#endif // __START_VIEW_H__
