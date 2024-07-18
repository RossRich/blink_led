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
absolute_time_t rate_timer = get_absolute_time();

class Controller : Subscriber {
private:
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
    if (is_radio_ok) {
      transition(fsm_state_t::START);
    }
    _start_time = get_absolute_time();
    return is_radio_ok;
  }

  void start_enter() { _view->start(); }

  void start() {
    if (_model->get_found().size()) {
      transition(fsm_state_t::WELL);
    }
  }

  void well_enter() { _view->well(); }

  void well() {}

  void error() {}

  void fsm_update() {
    if (current_state == fsm_state_t::TRANSITION)
      return;

    if (fsm_loop)
      fsm_loop();
  }

  // update from radio
  void update() override {
    if (mav_helper->parse_buf(_radio->buffer, _radio->pa_size)) {
      led_status = !led_status;
      gpio_put(LED_PIN, int(led_status));
    }
  }

  void run() {
    if (not time_reached(_start_time))
      return;

    _start_time = make_timeout_time_ms(radio_rate_ms);

    // printf("Controller rate: %u\n",
    //        1000 / us_to_ms(
    //                   absolute_time_diff_us(rate_timer,
    //                   get_absolute_time())));
    rate_timer = get_absolute_time();
    _radio->try_read();
    fsm_update();
    _view->update();
  }
};

#endif // __CONTROLLER_H__
