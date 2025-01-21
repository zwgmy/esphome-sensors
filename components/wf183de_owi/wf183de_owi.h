#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/one_wire/one_wire.h"

namespace esphome {
namespace wf183de_owi {

class WF183DE_OWI_Sensor : public PollingComponent, public one_wire::OneWireDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_pressure_sensor(sensor::Sensor *pressure_sensor) { pressure_sensor_ = pressure_sensor; }

 protected:
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *pressure_sensor_{nullptr};
  uint8_t scratch_pad_[9] = {0};

  /// Get the number of milliseconds we have to wait for the conversion phase.
  uint16_t millis_to_wait_for_conversion_() const;
  bool read_scratch_pad_();
  bool check_scratch_pad_();
  float get_temp_c_();
  float get_pressure_hpa_();
};

}  // namespace wf183de_owi
}  // namespace esphome
