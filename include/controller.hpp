#if !defined(__CONTROLLER_H__)
#define __CONTROLLER_H__

#define RADIO_RATE 60 //< Hz

#include "mavlink_helper.hpp"
#include "model.hpp"
#include "observer.hpp"
#include "pico/stdlib.h"
#include "radio.hpp"
#include "view.hpp"
#include <functional>

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint radio_rate_ms = uint((1.0f / RADIO_RATE) * 1000);
volatile bool is_radio_ok = false;
volatile bool led_status = false;

bool radio_callback(struct repeating_timer *t) {
  if (not is_radio_ok)
    return true;

  Radio *r = static_cast<Radio *>(t->user_data);
  r->try_read();

  return true;
}

class Controller : Subscriber {
private:
  struct repeating_timer radio_timer;
  Radio *_radio;
  Model *_model;
  View *_view;
  MavlinkHelper *mav_helper;

public:
  Controller(Model *model, View *view) : _model(model), _view(view) {
    mav_helper = new MavlinkHelper(model);
    _radio = new Radio();
    _radio->add_sub(this);
    add_repeating_timer_ms(radio_rate_ms, radio_callback, static_cast<void *>(_radio), &radio_timer);
  }
  virtual ~Controller() {}

  bool init() {
    is_radio_ok = _radio->init();
    return is_radio_ok;
  }

  // update from radio
  void update() override {
    led_status = !led_status;
    gpio_put(LED_PIN, int(led_status));

    if (mav_helper->parse_buf(_radio->buffer, 32)) {
      _view->update();
    }
  }
};

#endif // __CONTROLLER_H__
