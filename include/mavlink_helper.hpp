#if !defined(__MAVLINK_HELPER_H__)
#define __MAVLINK_HELPER_H__

#include "libs/mavlink2/common/mavlink.h"

#define MAV_SYS_ID 10
#define RADIO_CHAN MAVLINK_COMM_0

class MavlinkHelper {
private:
  mavlink_status_t _status;
  mavlink_message_t _msg;
  template <typename T>
  void _parse_msg(mavlink_message_t &mgs, T *) {}

public:
  MavlinkHelper() {}
  ~MavlinkHelper() {}

  void read(uint8_t *buf, size_t len) {
    for (uint8_t i = 0; i < len; ++i) {
      if (mavlink_parse_char(RADIO_CHAN, buf[i], &_msg, &_status)) {
        // что то делаем с сообщением
      }
    }
  }
};

#endif // __MAVLINK_HELPER_H__
