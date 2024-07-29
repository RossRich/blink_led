#if !defined(_PX4_H_)
#define _PX4_H_

#include "px4_custom_mode.h"
#include "static_data.hpp"
#include <string>
#include <unordered_map>

/* https://github.com/mavlink/mavros/blob/ros2/mavros/src/lib/uas_stringify.cpp
 */
/* -*- mode stringify functions -*- */

typedef std::unordered_map<uint32_t, const std::string> cmode_map;

//! PX4 custom mode -> string
static const cmode_map px4_cmode_map{{
    {px4::define_mode(px4::custom_mode::MAIN_MODE_MANUAL), FLY_MODES.manual},
    {px4::define_mode(px4::custom_mode::MAIN_MODE_ACRO), "ACRO"},
    {px4::define_mode(px4::custom_mode::MAIN_MODE_ALTCTL), FLY_MODES.altitude},
    {px4::define_mode(px4::custom_mode::MAIN_MODE_POSCTL), FLY_MODES.poctl},
    {px4::define_mode(px4::custom_mode::MAIN_MODE_OFFBOARD), FLY_MODES.offboard},
    {px4::define_mode(px4::custom_mode::MAIN_MODE_STABILIZED), FLY_MODES.stabilized},
    {px4::define_mode(px4::custom_mode::MAIN_MODE_RATTITUDE), "RATTITUDE"},
    {px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_MISSION), "AUTO.MISSION"},
    {px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_LOITER), "AUTO.LOITER"},
    {px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_RTL), "AUTO.RTL"},
    {px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_LAND), "AUTO.LAND"},
    {px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_RTGS), "AUTO.RTGS"},
    {px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_READY), "AUTO.READY"},
    {px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_TAKEOFF), "AUTO.TAKEOFF"},
    {px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_FOLLOW_TARGET),"AUTO.FOLLOW_TARGET"},
    {px4::define_mode_auto(px4::custom_mode::SUB_MODE_AUTO_PRECLAND),"AUTO.PRECLAND"},
}};

static inline std::string str_base_mode(int base_mode) {
  return FLY_MODES.no_data;
}

static std::string str_custom_mode(uint32_t custom_mode) {
  return FLY_MODES.no_data;
}

static std::string str_mode_cmap(const cmode_map &cmap, uint32_t custom_mode) {
  auto it = cmap.find(custom_mode);
  if (it != cmap.end())
    return it->second;
  else
    return str_custom_mode(custom_mode);
}

#endif // _PX4_H_
