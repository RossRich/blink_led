#if !defined(__UAV_MONITOR_H__)
#define __UAV_MONITOR_H__

#include "controller.hpp"
#include "model.hpp"
#include "radio.hpp"
#include "view.hpp"

class Monitor {
private:
  View *_view;
  Model *_model;
  Controller *_controller;

public:
  Monitor() {
    _model = new Model();
    _view = new View(_model);
    _controller = new Controller(_model, _view);
  }
  ~Monitor() {}

  bool init() { return _controller->init(); }
};

#endif // __UAV_MONITOR_H__
