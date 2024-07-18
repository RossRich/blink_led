#if !defined(__MAVLINK_HELPER_H__)
#define __MAVLINK_HELPER_H__

#include "libs/mavlink2/common/mavlink.h"
#include "model.hpp"
#include <pico/types.h>

#define MAV_SYS_ID 10
#define RADIO_CHAN MAVLINK_COMM_0

class MavlinkHelper {
private:
  Model *_model;

  void _decode_msg(mavlink_message_t &mgs) {
    if (msg.sysid > 3 or msg.sysid < 1)
      return;

    auto &isotope = _model->get_isotope(msg.sysid);
    if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
      mavlink_msg_heartbeat_decode(&msg, &isotope.msgs.heartbeat);
      isotope.is_updated = true;
    }
  }

public:
  mavlink_status_t status;
  mavlink_message_t msg;

  MavlinkHelper(Model *m) : _model(m) {}
  ~MavlinkHelper() {}

  bool parse_buf(uint8_t *buf, size_t len) {
    bool res = false;
    for (uint8_t i = 0; i < len; ++i) {
      if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status)) {
        _decode_msg(msg);
        res = true;
      }
    }

    return res;
  }
};

#endif // __MAVLINK_HELPER_H__
