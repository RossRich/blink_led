#if !defined(_CON_LOST_H_)
#define _CON_LOST_H_

#include "screen_base.hpp"

class ConnectionLost : public Screen {
private:
public:
  ConnectionLost(ScreenDriverI *driver) : Screen(driver) {}
  ~ConnectionLost() {}

  void on_start() override {
    static const char *CONNECTION = "Communication";
    static const char *LOST = "lost";

    write_str_row(CONNECTION, 4, screen_align_t::LEFT);
    write_str_row(LOST, 6, screen_align_t::RIGHT);
  }

  void on_draw() override {}
};

#endif // _CON_LOST_H_
