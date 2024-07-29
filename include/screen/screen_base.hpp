#if !defined(__SCREEN_BASE_H__)
#define __SCREEN_BASE_H__

#include "ssd1306_i2c.h"

#define SSD_LAST_PAGE (SSD1306_NUM_PAGES - 1u)
#define SSS_LAST_COL (SSD1306_WIDTH - 1u)
#define SCREEN_FPS 15u

enum screen_align_t : uint8_t {
  TOP = 1,
  BOTTOM = 1 << 1,
  LEFT = 1 << 2,
  RIGHT = 1 << 3,
  CENTER = 1 << 4
};

class ScreenDriverI {
public:
  virtual ~ScreenDriverI() {}
  virtual void write_str(int16_t x, int16_t y, const char *str,
                         struct render_area &frame) = 0;
  virtual void write_char(int16_t x, int16_t y, const uint8_t ch,
                          struct render_area &frame) = 0;
};

struct ScreenBase {
  static const uint8_t STR_SIZE = SSD1306_WIDTH / SSD1306_PAGE_HEIGHT;
  static const uint8_t STR_BUF_SIZE = STR_SIZE + 1;
  char str_buf[STR_BUF_SIZE];
};

class Screen {
private:
  struct render_area frame_area = {
    start_col : 0,
    end_col : SSD1306_WIDTH - 1,
    start_page : 0,
    end_page : SSD1306_NUM_PAGES - 1
  };

protected:
  absolute_time_t _update_timer = get_absolute_time();
  static ScreenBase sb;
  ScreenDriverI *_driver;
  uint16_t _update_rate_ms = uint16_t((1.0f / SCREEN_FPS) * 1000);
  uint32_t timex = millis();

public:
  Screen(ScreenDriverI *driver) : _driver(driver) {}
  ~Screen() {}

  void write_str(const char *str, uint8_t x, uint8_t y) {
    render_area ra;

    _driver->write_str(x > 0 ? (x * SSD1306_PAGE_HEIGHT) - 1 : 0, y * SSD1306_PAGE_HEIGHT, str, ra);
  }

  void write_str_row(const char *str, uint8_t row, screen_align_t align,
                     bool is_preclean = false) {
    uint8_t x = 0;
    auto len = strlen(str);

    // left, center, right
    if (align & screen_align_t::CENTER) {
      uint8_t m = len % 2 ? uint8_t(len / 2) + 1 : uint8_t(len / 2);
      x = 8u - m;
    } else if (align & screen_align_t::RIGHT) {
      auto width_len = SSD1306_WIDTH / SSD1306_PAGE_HEIGHT;
      x = len > width_len ? 0 : width_len - len;
    } else if (align & screen_align_t::LEFT) {
      x = 0;
    }

    write_str(str, x, row);
  }

  /**
   * x - колонка, y - строка в размерностях символов (8*8)
   */
  void delete_str(uint8_t x, uint8_t y, uint8_t len = 0) {
    if (len > sb.STR_SIZE or x > (sb.STR_SIZE - 1)) return;
    
    if (not len and not x)
      len = sb.STR_SIZE;
    else if (len == 0)
      len = sb.STR_SIZE - x;

    memset(sb.str_buf, ' ', len);
    sb.str_buf[len] = '\0';
    write_str(sb.str_buf, x, y);
  }

  void draw() {
    if (not time_reached(_update_timer))
      return;

    _update_timer = make_timeout_time_ms(_update_rate_ms);
    // printf("draw: %lu\n", millis() - timex);
    timex = millis();

    on_draw();
  }

  virtual void on_start() {}

  virtual void on_draw() = 0;
};

ScreenBase Screen::sb;

#endif // __SCREEN_BASE_H__
