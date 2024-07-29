#if !defined(_ISOTOPE_CLASS_H__)
#define _ISOTOPE_CLASS_H__

#include "libs/mavlink2/minimal/mavlink.h"
#include "px4.hpp"
#include "static_data.hpp"
#include <pico/time.h>
#include <string>
#include <unordered_map>

struct mavlink_msgs_t {
  mavlink_heartbeat_t heartbeat;
};

class Isotope {
private:
  const uint8_t _sys_id;
  static const uint16_t _stable_connection_timout =
      1000; //< ms, максимальная задержка между получением пакетов сообщений
  absolute_time_t _stable_connection_timer = nil_time;

public:
  mavlink_msgs_t msgs;
  bool is_updated = false;
  const char *name = nullptr;
  Isotope() = delete;
  explicit Isotope(const uint8_t sys_id, const char *name)
      : _sys_id(sys_id), name(name) {}
  ~Isotope() {}

  inline void update_time() { _stable_connection_timer = get_absolute_time(); }

  inline uint8_t id() const { return _sys_id; }

  bool is_connection_ok() const {
    if (_stable_connection_timer == nil_time)
      return false;

    return us_to_ms(absolute_time_diff_us(_stable_connection_timer,
                                          get_absolute_time())) <=
           _stable_connection_timout;
  }

  const char *fly_mode() const {
    const char *m = FLY_MODES.no_data;
    auto base_mode = msgs.heartbeat.base_mode;
    auto custom_mode_int = msgs.heartbeat.custom_mode;

    if (not(base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED))
      return m;

    px4::custom_mode custom_mode(custom_mode_int);
    return str_mode_cmap(px4_cmode_map, custom_mode.data).c_str();
  }

  const char *arm_status() const {
    return msgs.heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED
               ? ARM_STATUS.arm
               : ARM_STATUS.disarm;
  }
};

#endif // _ISOTOPE_CLASS_H__
