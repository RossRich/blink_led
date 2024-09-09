#if !defined(__MODEL_H__)
#define __MODEL_H__

#define MAX_ISOTOPES 3u
/* проверять имеются ли подключенные устройства 1 раз в 1000 ms */
#define CHECK_CONNECTION_DT 2000u

#include "isotope.hpp"
#include "libs/mavlink2/minimal/mavlink.h"
#include <array>
#include <list>
#include <pico/types.h>

class MavlinkHelper;

class Model {
private:
  std::array<Isotope *, MAX_ISOTOPES> _isotopes_array = {nullptr};
  std::list<uint8_t> sys_found;
  MavlinkHelper *_mav_helper;
  absolute_time_t _check_connection_timer = nil_time;
  bool _is_con_ok = false;

  inline size_t sys_id_to_idx(const uint8_t sys_id) const {
    return sys_id - 1; //< sys_id начинаются с 1, а индексация массива с 0
  }

public:
  size_t msg_counter = 0;

  Model();
  ~Model() {}

  void parse_buf(const uint8_t *buf, uint len);

  bool init() { return true; }

  bool is_exist(const uint8_t sys_id) {
    auto idx = sys_id_to_idx(sys_id);

    if (_isotopes_array[idx]) return true;

    return false;
  }

  inline const std::list<uint8_t> &get_found() const { return sys_found; }

  Isotope &get_isotope(const uint8_t sys_id) {
    auto idx = sys_id_to_idx(sys_id);

    if (not _isotopes_array[idx]) {
      _isotopes_array[idx] =
          new Isotope(sys_id, ISOTOPE_NAMES.get_name_by_id(sys_id));
      sys_found.push_back(sys_id);
    }

    return *_isotopes_array[idx];
  }

  /**
   * Имеется ли связь с каким либо устройством
   */
  bool is_connection_ok() {
    if (not time_reached(_check_connection_timer)) return _is_con_ok;

    _is_con_ok = false;
    for (auto &sys_id : sys_found) {
      auto &isotope = get_isotope(sys_id);
      // printf("Isotope #%u link: %u\n", uint(isotope.id()),
      // uint(isotope.is_connection_ok()));
      if (isotope.is_connection_ok()) {
        _is_con_ok = true;
        break;
      }
    }

    _check_connection_timer = make_timeout_time_ms(CHECK_CONNECTION_DT);

    return _is_con_ok;
  }

  friend MavlinkHelper;
};

class MavlinkHelper {
private:
  Model *_model;
  void _decode_msg(mavlink_message_t &mgs) {
    if (msg.sysid > 3 or msg.sysid < 1) return;

    auto &isotope = _model->get_isotope(msg.sysid);
    isotope.update_time();
    if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
      mavlink_msg_heartbeat_decode(&msg, &isotope.msgs.heartbeat);
      isotope.is_updated = true;
    }
  }

public:
  mavlink_status_t status;
  mavlink_message_t msg;

  MavlinkHelper() = delete;
  explicit MavlinkHelper(Model *m) : _model(m) {}
  ~MavlinkHelper() {}

  bool parse_buf(const uint8_t *buf, uint len) {
    bool res = false;
    // printf("parse %u bytes\n", len);
    for (uint i = 0; i < len; ++i) {
      if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status)) {
        _decode_msg(msg);
        _model->msg_counter++;
        if (_model->msg_counter >= 9999) _model->msg_counter = 1;
        res = true;
      }
    }

    return res;
  }
};

Model::Model() { _mav_helper = new MavlinkHelper(this); }

void Model::parse_buf(const uint8_t *buf, uint len) {
  _mav_helper->parse_buf(buf, len);
}

#endif // __MODEL_H__
