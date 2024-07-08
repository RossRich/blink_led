#if !defined(__MODEL_H__)
#define __MODEL_H__

#include "libs/c_library_v2/minimal/mavlink.h"
#include "observer.hpp"

class Model : public Publisher, Subscriber {
private:
  size_t _msg_count = 0;
  mavlink_heartbeat_t hb_msg;

public:
  Model() {}

  ~Model() {}

  bool init() { return true; }

  int inc_counter() { return ++_msg_count; }

  void update() override {
    // data from radoi
  }
};

#endif // __MODEL_H__
