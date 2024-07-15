#if !defined(__STATIC_DATA_H__)
#define __STATIC_DATA_H__

static struct isotope_names_t {
  const char *protium = "prt";
  const char *deuterium = "det";
  const char *tritium = "trt";
} ISOTOPE_NAMES;

static struct fly_modes_t {
  const char *poctl = "POCTL";
  const char *offboard = "OFFBD";
  const char *no_data = "NODAT";
  const char *manual = "MANUL";
  const char *altitude = "ALTTD";
  const char *stabilized = "STABL";
} FLY_MODES;

static const struct arm_status_t {
  const char *arm = "arm";
  const char *disarm = "safe";
} ARM_STATUS;

#endif // _STAT_IC_DATA_H__
