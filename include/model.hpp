#if !defined(__MODEL_H__)
#define __MODEL_H__

#define MAX_ISOTOPES 3u

#include "isotope.hpp"
#include "libs/mavlink2/minimal/mavlink.h"
#include <array>
#include <list>

class Model {
private:
  std::array<Isotope *, MAX_ISOTOPES> _isotopes_array = {nullptr};
  std::list<uint8_t> sys_found;

  inline size_t sys_id_to_idx(const uint8_t sys_id) const {
    return sys_id - 1; //< sys_id начинаются с 1, а индексация массива с 0
  }

public:
  Model() {}
  ~Model() {}

  bool init() { return true; }

  bool is_exist(const uint8_t sys_id) {
    auto idx = sys_id_to_idx(sys_id);

    if (_isotopes_array[idx])
      return true;

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
};

#endif // __MODEL_H__
