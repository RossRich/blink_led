#if !defined(__MODEL_H__)
#define __MODEL_H__

#include "libs/mavlink2/minimal/mavlink.h"
#include <array>

class Model {
private:
  size_t _msg_count = 0;
  std::array<uint8_t, 10> sys_ids;
  uint8_t ids_count = 0;

public:
  struct mavlink_msgs {
    uint8_t sys_id = 0;
    bool is_updated = false;
    mavlink_heartbeat_t heartbeat;
  } msgs;

  std::array<mavlink_msgs, 3> msg_container;

  Model() {}

  ~Model() {}

  bool init() { return true; }

  void add_sys_id(uint8_t id) {
    if (ids_count < sys_ids.max_size()) {
      for (size_t i = 0; i < sys_ids.size(); ++i) {
        if (sys_ids[i] == id)
          break;
      }
      sys_ids[ids_count++] = id;
    }
  }

  const uint8_t *get_sys_ids() { return sys_ids.data(); }

  void inc_counter() { ++_msg_count; }
  int get_counter() { return _msg_count; }
};

#endif // __MODEL_H__
