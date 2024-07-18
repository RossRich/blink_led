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
    auto found = _model->get_found();

    if (not found.size()) {
      write_str_row(NO_DATA_ERROR, 4, screen_align_t::CENTER);
      return;
    }

    uint8_t row = 3;
    for (auto &sys_id : found) {
      uint8_t cursor = 0;
      auto &isotope = _model->get_isotope(sys_id);
      write_str(isotope.name, cursor, row * SSD1306_PAGE_HEIGHT);
      cursor = (strlen(isotope.name) + 1);
      write_str(FLY_MODES.no_data, cursor * SSD1306_PAGE_HEIGHT, row * SSD1306_PAGE_HEIGHT);
      cursor = SSD1306_WIDTH - 1 - SSD1306_PAGE_HEIGHT - strlen(ARM_STATUS.disarm) * SSD1306_PAGE_HEIGHT;
      write_str(ARM_STATUS.disarm, cursor, row * SSD1306_PAGE_HEIGHT);
      row += 2;
    }
  }

  void on_draw() override {
    auto found = _model->get_found();
    if (not found.size()) return;
    
    uint8_t row = 3;
    for (auto &sys_id : found) {
      auto &isotope = _model->get_isotope(sys_id);
      if (not isotope.is_updated) continue;
      isotope.is_updated = false;
      uint8_t cursor = (strlen(isotope.name) + 1);
      printf("name: %s\nfl_mode: %s\narm: %s\n", isotope.name, isotope.fly_mode(), isotope.arm_status());
      write_str(isotope.fly_mode(), cursor * SSD1306_PAGE_HEIGHT, row * SSD1306_PAGE_HEIGHT);
      cursor = SSD1306_WIDTH - 1 - SSD1306_PAGE_HEIGHT - strlen(isotope.arm_status()) * SSD1306_PAGE_HEIGHT;
      write_str(isotope.arm_status(), cursor, row * SSD1306_PAGE_HEIGHT);
      row += 2;
    }
  }
};

#endif // __SCREEN_H__
