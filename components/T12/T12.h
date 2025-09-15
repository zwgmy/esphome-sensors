#pragma once

#include "esphome.h"

namespace esphome {
namespace t12_temperature_sensor {

class T12TemperatureSensor : public sensor::Sensor, public PollingComponent {
 public:
  T12TemperatureSensor();
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_offset_voltage(float offset) { offset_voltage_mv_ = offset; }
  void set_amplification_factor(float factor) { amplification_factor_ = factor; }

  // 电压-温度对应表 (电压单位: mV, 温度单位: °C)
  static const float voltage_points[11];
  static const float temperature_points[11];
  static const int table_size;

 protected:
  float lookup_temperature(float voltage_mv);

  float offset_voltage_mv_;
  float amplification_factor_;
  unsigned long last_heating_time_;
};

}  // namespace t12_temperature_sensor
}  // namespace esphome
