#if !defined(_ISOTOPE_CLASS_H__)
#define _ISOTOPE_CLASS_H__

#include "libs/mavlink2/minimal/mavlink.h"
#include "px4_custom_mode.h"
#include "static_data.hpp"
#include <unordered_map>
#include <string>

struct mavlink_msgs_t {
  mavlink_heartbeat_t heartbeat;
};


/* https://github.com/mavlink/mavros/blob/ros2/mavros/src/lib/uas_stringify.cpp */
/* -*- mode stringify functions -*- */

typedef std::unordered_map<uint32_t, const std::string> cmode_map;

//! PX4 custom mode -> string
static const cmode_map px4_cmode_map{{
	{ px4::define_mode(px4::custom_mode::MAIN_MODE_MANUAL),           FLY_MODES.manual },
	{ px4::define_mode(px4::custom_mode::MAIN_MODE_ACRO),             "ACRO" },
	{ px4::define_mode(px4::custom_mode::MAIN_MODE_ALTCTL),           FLY_MODES.altitude },
	{ px4::define_mode(px4::custom_mode::MAIN_MODE_POSCTL),           FLY_MODES.poctl },
	{ px4::define_mode(px4::custom_mode::MAIN_MODE_OFFBOARD),         FLY_MODES.offboard },
	{ px4::define_mode(px4::custom_mode::MAIN_MODE_STABILIZED),       FLY_MODES.stabilized },
	{ px4::define_mode(px4::custom_mode::MAIN_MODE_RATTITUDE),        "RATTITUDE" },
	{ px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_MISSION), "AUTO.MISSION" },
	{ px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_LOITER),  "AUTO.LOITER" },
	{ px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_RTL),     "AUTO.RTL" },
	{ px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_LAND),    "AUTO.LAND" },
	{ px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_RTGS),    "AUTO.RTGS" },
	{ px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_READY),   "AUTO.READY" },
	{ px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_TAKEOFF), "AUTO.TAKEOFF" },
	{ px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_FOLLOW_TARGET), "AUTO.FOLLOW_TARGET" },
	{ px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_PRECLAND), "AUTO.PRECLAND" },
}};

static inline std::string str_base_mode(int base_mode) {
	return FLY_MODES.no_data;
}

static std::string str_custom_mode(uint32_t custom_mode) {
	return FLY_MODES.no_data;
}

static std::string str_mode_cmap(const cmode_map &cmap, uint32_t custom_mode)
{
	auto it = cmap.find(custom_mode);
	if (it != cmap.end())
		return it->second;
	else
		return str_custom_mode(custom_mode);
}

class Isotope {
private:
  const uint8_t _sys_id;

public:
  mavlink_msgs_t msgs;
  bool is_updated = false;
  const char *name = nullptr;
  Isotope() = delete;
  explicit Isotope(const uint8_t sys_id, const char *name)
      : _sys_id(sys_id), name(name) {}
  ~Isotope() {}

  const char *fly_mode() const {
    const char *m = FLY_MODES.no_data;
    auto base_mode = msgs.heartbeat.base_mode;
    auto custom_mode_int = msgs.heartbeat.custom_mode;

    printf("bm: %u, cm: %u\n", base_mode, custom_mode_int);

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
