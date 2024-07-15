#if !defined(__SCREEN_H__)
#define __SCREEN_H__

#include "screen_base.hpp"

class WellView : public Screen {
private:
  uint counter = 0;

public:
  WellView(ScreenDriverI *driver, Model *model) : Screen(driver, model) {}
  virtual ~WellView() {}

  void on_start() {
    const char *test = "well on start";
    write_str(test, 0, 4);
    printf(test);
  }

  void update() override {
    sprintf(Screen::sb.str_buf, "wall: %d  ", counter);
    write_str(sb.str_buf, 0, 4);
    counter += 1;
  }
};

#endif // __SCREEN_H__
