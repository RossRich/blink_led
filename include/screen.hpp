#if !defined(__SCREEN_H__)
#define __SCREEN_H__

#include "model.hpp"
#include "ssd1306_i2c.h"
#include <ctype.h>

#define SSD_LAST_PAGE (SSD1306_NUM_PAGES - 1u)
#define SSS_LAST_COL (SSD1306_WIDTH - 1u)

static struct isotope_names_t {
  const char *protium = "prt";
  const char *deuterium = "dut";
} ISOTOPE_NAMES;

static struct fly_modes_t {
  const char *poctl = "POCTL";
  const char *offboard = "OFFBD";
  const char *no_data = "NODAT";
  const char *manual = "MANUL";
  const char *altitude = "ALTTD";
  const char *stabilized = "STABL";
} FLY_MODES;

class ScreenDriverI {
public:
  virtual ~ScreenDriverI() {}
  virtual void write_str(int16_t x, int16_t y, const char *str,
                         struct render_area &frame) = 0;
  virtual void write_char(int16_t x, int16_t y, const uint8_t ch,
                          struct render_area &frame) = 0;
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

public:
  Screen(ScreenDriverI *driver, Model *model)
      : _driver(driver), _model(model) {}
  ~Screen() {}

  void on_start() {
    write_str(ISOTOPE_NAMES.protium, 0, 4);
    write_str(ISOTOPE_NAMES.deuterium, 0, 6);
    write_str(FLY_MODES.no_data, (strlen(ISOTOPE_NAMES.protium) + 1) * 8, 4);
    write_str(FLY_MODES.no_data, (strlen(ISOTOPE_NAMES.deuterium) + 1) * 8, 6);
  }

  void write_str(const char *str, uint8_t x, uint8_t y) {
    size_t len = strlen(str);

    render_area ra;
    ra.start_col = x;
    ra.end_col = len * SSD1306_PAGE_HEIGHT + x;
    ra.start_page = y;
    ra.end_page = y;
    _driver->write_str(x, y, str, ra);
  }

  void update() {
    auto &sys_msg = _model->msg_container.at(1);
    if (not sys_msg.is_updated)
      return;

    sys_msg.is_updated = false;

    auto &hb = sys_msg.heartbeat;

    bool is_armed = hb.base_mode & MAV_MODE_FLAG::MAV_MODE_FLAG_SAFETY_ARMED;
    bool is_guided = hb.base_mode & MAV_MODE_FLAG::MAV_MODE_FLAG_GUIDED_ENABLED;
    bool is_manual_input =
        hb.base_mode & MAV_MODE_FLAG::MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;
    printf("STATE: %d\n", hb.system_status);
    printf("IS_ARM: %d\n", is_armed);
    printf("CM: %d\n", hb.custom_mode);
    printf("BM: %d\n", hb.base_mode);

    const char *m = FLY_MODES.no_data;
    if (hb.custom_mode & 1) {
      m = FLY_MODES.poctl;
    } else if (hb.custom_mode & 2) {
      m = FLY_MODES.manual;
    }

    printf("MODE: %s\n", m);

    write_str(m, (strlen(ISOTOPE_NAMES.protium) + 1) * 8, 4);
  }
};

#endif // __SCREEN_H__
