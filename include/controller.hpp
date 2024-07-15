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
#include <unordered_map>

enum fsm_state_t { TRANSITION, START, WELL, ERROR };

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
  absolute_time_t _start_time = nil_time;
  absolute_time_t print_timer = get_absolute_time();

  std::function<void(void)> fsm_loop = nullptr;
  std::unordered_map<fsm_state_t, std::function<void(void)>> _states;
  fsm_state_t current_state = fsm_state_t::TRANSITION;

  void transition(fsm_state_t new_state) {
    if (new_state == current_state)
      return;

    printf("Transition from %d to %d\n", uint(current_state), uint(new_state));

    current_state = fsm_state_t::TRANSITION;
    fsm_loop = _states.at(new_state);

    switch (new_state) {
    case fsm_state_t::START:
      start_enter();
      current_state = fsm_state_t::START;
      break;

    case fsm_state_t::WELL:
      well_enter();
      current_state = fsm_state_t::WELL;
      break;

    case fsm_state_t::ERROR:
      current_state = fsm_state_t::ERROR;
      break;

    default:

      break;
    }
  }

public:
  Controller(Model *model, View *view) : _model(model), _view(view) {
    mav_helper = new MavlinkHelper(model);
    _radio = new Radio();
    _radio->add_sub(this);
    add_repeating_timer_ms(radio_rate_ms, radio_callback,
                           static_cast<void *>(_radio), &radio_timer);

    _states.insert(std::make_pair(fsm_state_t::START,
                                  std::bind(&Controller::start, this)));
    _states.insert(std::make_pair(fsm_state_t::ERROR,
                                  std::bind(&Controller::error, this)));
    _states.insert(
        std::make_pair(fsm_state_t::WELL, std::bind(&Controller::well, this)));
  }
  virtual ~Controller() {}

  bool init() {
    is_radio_ok = _radio->init();
    return is_radio_ok;
  }

  void print_trottled(const char *msg) {
    if (print_timer > get_absolute_time())
      return;

    printf(msg);

    print_timer = make_timeout_time_ms(1000);
  }

  void start_enter() {
    printf("start enter\n");
    _view->start();
    _start_time = make_timeout_time_ms(5000);
  }

  void start() {
    if (_start_time < get_absolute_time()) {
      transition(fsm_state_t::WELL);
      return;
    }

    char buf[50];
    sprintf(
        buf, "Next state after: %d\n",
        int(us_to_ms(absolute_time_diff_us(get_absolute_time(), _start_time))) /
            1000);
    print_trottled(buf);
  }

  void well_enter() {
    _view->well();
    _start_time = make_timeout_time_ms(5000);
    printf("well enter\n");
  }
  void well() {
    if (_start_time > get_absolute_time()) {
      char buf[50];
      sprintf(buf, "Next state after: %d\n",
              uint(us_to_ms(
                  absolute_time_diff_us(get_absolute_time(), _start_time))) /
                  1000);
      print_trottled(buf);
    } else {
      transition(fsm_state_t::START);
    }
  }

  void error() {}

  void fsm_update() {
    if (current_state == fsm_state_t::TRANSITION)
      return;

    if (fsm_loop != nullptr)
      fsm_loop();
  }

  // update from radio
  void update() override {
    if (is_nil_time(_start_time)) {
      transition(fsm_state_t::START);
    }

    led_status = !led_status;
    gpio_put(LED_PIN, int(led_status));

    fsm_update();
    _view->update();

    if (mav_helper->parse_buf(_radio->buffer, 32)) {
      // _view->update();
    }
  }
};

#endif // __CONTROLLER_H__
