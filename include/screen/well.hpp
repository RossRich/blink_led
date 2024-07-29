#if !defined(__SCREEN_H__)
#define __SCREEN_H__

#include "../model.hpp"
#include "screen_base.hpp"

class WellView : public Screen {
private:
  uint counter = 0;
  Model *_model;
  uint8_t _blinked_rows = 0;
  absolute_time_t _blink_timer = get_absolute_time();
  bool _blink_status = false;

  // void update_row_blinker(Isotope &isotope) {
  //   uint8_t offset = 2;
  //   uint8_t start_row = 3;
  //   uint8_t row = (isotope.id() - 1) * offset + start_row;
  //   uint8_t row_bit = 1 << row;
  //   not isotope.is_connection_ok() ? _blinked_rows |= row_bit
  //                                  : _blinked_rows &= ~row_bit;
  // }

public:
  WellView(ScreenDriverI *driver, Model *model)
      : Screen(driver), _model(model) {}
  virtual ~WellView() {}

  void on_start() {
    auto found = _model->get_found();
    _blinked_rows = 0;

    if (not found.size()) {
      write_str_row(NO_DATA_ERROR, 4, screen_align_t::CENTER);
      return;
    }

    for (auto &sys_id : found) {
      auto &isotope = _model->get_isotope(sys_id);
      isotope_to_row(isotope);
    }

    sprintf(sb.str_buf, "msgs count %lu", _model->msg_counter);
    write_str(sb.str_buf, 0, 0);
  }

  void on_draw() override {
    sprintf(sb.str_buf, "msgs count %lu", _model->msg_counter);
    write_str(sb.str_buf, 0, 0);

    auto found = _model->get_found();
    if (not found.size())
      return;
    for (auto &sys_id : found) {
      auto &isotope = _model->get_isotope(sys_id);
      // update_row_blinker(isotope);
      if (not isotope.is_updated and isotope.is_connection_ok())
        continue;
      isotope_to_row(isotope);
      isotope.is_updated = false;
    }

    if (time_reached(_blink_timer)) {
      _blink_status = !_blink_status;
      _blink_timer = make_timeout_time_ms(1 / 1 * 1000);
      // printf("blinked_row: %u\n", _blinked_rows);
    }
  }

  void isotope_to_row(Isotope &isotope) {
    uint8_t offset = 2;
    uint8_t start_row = 3;
    uint8_t row = (isotope.id() - 1) * offset + start_row;

    if (row > SSD1306_NUM_PAGES - 1 or row < 0)
      return;

    uint8_t cursor = 0;
    write_str(isotope.name, cursor, row);
    cursor = strlen(isotope.name) + 1;
    uint8_t row_bit = 1 << row;

    not isotope.is_connection_ok() ? _blinked_rows |= row_bit
                                   : _blinked_rows &= ~row_bit;

    if (_blinked_rows & row_bit) {
      if (_blink_status) {
        write_str(isotope.fly_mode(), cursor, row);
        write_str_row(isotope.arm_status(), row, screen_align_t::RIGHT);
      } else {
        delete_str(cursor, row);
      }
    } else {
      write_str(isotope.fly_mode(), cursor, row);
      write_str_row(isotope.arm_status(), row, screen_align_t::RIGHT);
    }
  }
};

#endif // __SCREEN_H__
