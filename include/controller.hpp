#if !defined(__CONTROLLER_H__)
#define __CONTROLLER_H__

#define RADIO_RATE 1 //< Hz

#include "model.hpp"
#include "observer.hpp"
#include "pico/stdlib.h"
#include "radio.hpp"
#include "view.hpp"

volatile bool is_radio_ok = false;

bool radio_callback(struct repeating_timer *t) {
  if (not is_radio_ok)
    return true;
  // gpio_put(LED_PIN, 1);
  Radio *r = static_cast<Radio *>(t->user_data);
  r->try_read();
  // gpio_put(LED_PIN, 0);

  return true;
}

class Controller : Subscriber {
private:
  struct repeating_timer radio_timer;
  Radio *_radio;
  Model *_model;
  View *_view;

public:
  Controller() = delete;
  Controller(Model *model, View *view) : Subscriber(), _model(model), _view(view) {
    _radio = new Radio();
    _radio->add(this);
    add_repeating_timer_ms(RADIO_RATE * 1000, radio_callback,
                           static_cast<void *>(_radio), &radio_timer);
  }
  virtual ~Controller() {}

  bool init() {
    is_radio_ok = _radio->init();
    _model->init();
    _view->init();
    return is_radio_ok;
  }

  void update() override { 
    _view->update_counter(_model->inc_counter());
  }

  void parse_msg() {

  }
};

#endif // __CONTROLLER_H__
