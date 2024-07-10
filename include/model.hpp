#if !defined(__MODEL_H__)
#define __MODEL_H__

#include "libs/mavlink2/minimal/mavlink.h"
#include "mavlink_helper.hpp"
#include "observer.hpp"

class Model : public Publisher, Subscriber {
private:
  size_t _msg_count = 0;
  struct mavlink_msgs {
    mavlink_heartbeat_t heartbeat;
  } _msgs;

public:
  Model() {}

  ~Model() {}

  bool init() { return true; }

  int inc_counter() { return ++_msg_count; }

  bool update_msg(mavlink_message_t &msg) {
    if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
      mavlink_msg_heartbeat_decode(&msg, &_msgs.heartbeat);
    } else {
      return false;
    }

    return true;
  }

  void update() override {
    // data from radoi
  }
};

#endif // __MODEL_H__
