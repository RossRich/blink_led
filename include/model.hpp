#if !defined(__MODEL_H__)
#define __MODEL_H__

#include "libs/mavlink2/minimal/mavlink.h"

class Model {
private:
  size_t _msg_count = 0;

public:
  struct mavlink_msgs {
    mavlink_heartbeat_t heartbeat;
  } msgs;

  Model() {}

  ~Model() {}

  bool init() { return true; }

  void inc_counter() { ++_msg_count; }
  int get_counter() { return _msg_count; }
};

#endif // __MODEL_H__
