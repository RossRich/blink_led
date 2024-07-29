#if !defined(__VIEW_H__)
#define __VIEW_H__

#include "model.hpp"
#include "pico/stdlib.h"
#include "screen/con_lost.hpp"
#include "screen/screen_base.hpp"
#include "screen/start.hpp"
#include "screen/well.hpp"

class View {
private:
  class Driver : public ScreenDriverI {
  private:
    uint8_t _buffer[SSD1306_BUF_LEN];
    void _init_ssd1306() { SSD1306_init(); }

  public:
    Driver() {
      _init_ssd1306();
      memset(_buffer, 0, SSD1306_BUF_LEN);
    }
    ~Driver() override {}

    void clear() {
      struct render_area rd;
      rd.start_col = 0;
      rd.end_col = SSD1306_WIDTH - 1;
      rd.start_page = 0;
      rd.end_page = SSD1306_NUM_PAGES - 1;
      calc_render_area_buflen(&rd);
      memset(_buffer, 0, SSD1306_BUF_LEN);
      render(_buffer, &rd);
    }

    void write_str(int16_t x, int16_t y, const char *str,
                   render_area &frame) override {
      WriteString(_buffer, x, y, str);

      render_area ra;
      ra.start_col = x;
      ra.end_col = strlen(str) * SSD1306_PAGE_HEIGHT + x;
      ra.start_page = y / 8;
      ra.end_page = y / 8;
      calc_render_area_buflen(&ra);
      /* TODO: Избавится от лишнего выделения памяти */
      uint8_t *temp_buf = static_cast<uint8_t *>(malloc(ra.buflen + 1));
      temp_buf[0] = 0x40;
      size_t idx = y / 8 * 128 + x;
      memcpy(temp_buf + 1, _buffer + idx, ra.buflen);
      render2(temp_buf, &ra);
      free(temp_buf);
    };
    void write_char(int16_t x, int16_t y, const uint8_t ch,
                    struct render_area &frame) override{};
  } * _driver;

  Screen *_start;
  Screen *_well;
  Screen *_con_lost;
  Screen *_visible;
  Model *_model;

public:
  View(Model *model) : _model(model) {
    _driver = new Driver();
    _start = new StartView(_driver);
    _con_lost = new ConnectionLost(_driver);
    _well = new WellView(_driver, model);
    start();
  }

  ~View() {}

  void well() {
    _driver->clear();
    _visible = _well;
    _visible->on_start();
  }

  void start() {
    _driver->clear();
    _visible = _start;
    _visible->on_start();
  }

  void connection_lost() {
    _driver->clear();
    _visible = _con_lost;
    _visible->on_start();
  }

  void update() {
    if (not _visible)
      return;

    _visible->draw();
  }
};

#endif // __VIEW_H__
