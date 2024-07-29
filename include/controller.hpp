#if !defined(__CONTROLLER_H__)
#define __CONTROLLER_H__

#define RADIO_RATE 90 //< Hz
#define ENC_SW 13u
#define ENC_CLK 12u
#define ENC_DT 11u

#include "model.hpp"
#include "pico/stdlib.h"
#include "radio.hpp"
#include "view.hpp"
#include <functional>
#include <memory>
#include <unordered_map>

uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint radio_rate_ms = uint((1.0f / RADIO_RATE) * 1000);
volatile bool is_radio_ok = false;
volatile bool led_status = false;
absolute_time_t rate_timer = get_absolute_time();

class Controller {
  class FSM {
    friend Controller;
    static constexpr uint FN_IDX = 1;
    enum fsm_state_t { TRANSITION, START, WELL, ERROR };
    struct transition_t {
      std::function<void(void)> on_enter;
      std::function<void(void)> loop;
      std::function<void(void)> on_exit;
    };

    std::unordered_map<fsm_state_t, transition_t> _states;
    fsm_state_t current_state = fsm_state_t::TRANSITION;
    std::function<void(void)> *fsm_loop = nullptr;

    void transition(fsm_state_t new_state) {
      if (new_state == current_state)
        return;

      auto cur_trs = _states.find(current_state);
      auto &trs = _states.at(new_state);
      current_state = fsm_state_t::TRANSITION;
      if (cur_trs != _states.end())
        std::get<FN_IDX>(*cur_trs).on_exit();
      trs.on_enter();
      fsm_loop = &trs.loop;
      current_state = new_state;
    }

  public:
    FSM() {}
    ~FSM() {}

    void loop() {
      if (current_state == fsm_state_t::TRANSITION)
        return;

      if (fsm_loop)
        (*fsm_loop)();
    }
  };

  using fsm_state_t = FSM::fsm_state_t;
  using fsm_trs_t = FSM::transition_t;

  FSM _fsm;

  std::unique_ptr<Radio> _radio;
  std::unique_ptr<Model> _model;
  std::unique_ptr<View> _view;

  absolute_time_t _start_time = nil_time;
  absolute_time_t print_timer = get_absolute_time();

  void make_fsm() {
    fsm_trs_t start{
      on_enter : std::bind(&Controller::start_enter, this),
      loop : std::bind(&Controller::start, this),
      on_exit : []() { return; }
    };

    _fsm._states.insert(std::make_pair(fsm_state_t::START, std::move(start)));

    fsm_trs_t well{
      on_enter : std::bind(&Controller::well_enter, this),
      loop : std::bind(&Controller::well, this),
      on_exit : []() { return; }
    };

    _fsm._states.insert(std::make_pair(fsm_state_t::WELL, std::move(well)));

    fsm_trs_t error{
      on_enter : std::bind(&Controller::error_enter, this),
      loop : std::bind(&Controller::error, this),
      on_exit : []() { return; }
    };

    _fsm._states.insert({fsm_state_t::ERROR, std::move(error)});
  }

  void read_radio_callback(const uint8_t *data, uint len) {
    // if (mav_helper->parse_buf(_radio->buffer, RADIO_BUFFER_SIZE)) {
    _model->parse_buf(data, len);
    led_status = !led_status;
    gpio_put(LED_PIN, int(led_status));
    // }
  }

public:
  Controller() {
    make_fsm();
    _model = std::make_unique<Model>(); //< share_pointer
    _view = std::make_unique<View>(_model.get());
    _radio = std::make_unique<Radio>();
    _radio->add_read_callback(std::bind(&Controller::read_radio_callback, this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));
  }

  virtual ~Controller() {}

  bool init() {
    _start_time = get_absolute_time();
    bool is_model_ok = _model->init();
    bool is_radio_ok = _radio->init();

    bool res = is_model_ok and is_radio_ok;

    if (res)
      _fsm.transition(fsm_state_t::START);

    return res;
  }

  void start_enter() { _view->start(); }

  void start() {
    /* если есть полдключение и пришли полезные данные */
    if (_model->is_connection_ok() and _model->get_found().size()) {
      for (auto &id : _model->get_found()) {
        auto &i = _model->get_isotope(id);
        if (i.is_updated)
          _fsm.transition(fsm_state_t::WELL);
      }
    }
  }

  void well_enter() { _view->well(); }

  void well() {
    if (not _model->is_connection_ok())
      _fsm.transition(fsm_state_t::ERROR);
  }

  void error_enter() { _view->connection_lost(); }

  void error() {
    if (_model->is_connection_ok())
      _fsm.transition(fsm_state_t::WELL);
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
    _fsm.loop();
    _view->update();
  }
};

#endif // __CONTROLLER_H__
