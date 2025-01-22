#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace wf183de_uart {

class WF183DE_UART : public PollingComponent, public uart::UARTDevice {
 public:
  WF183DE_UART() = default;

  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_pressure_sensor(sensor::Sensor *pressure_sensor) { pressure_sensor_ = pressure_sensor; }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *pressure_sensor_{nullptr};

  void write_command_(uint8_t command);
  bool read_response_(uint8_t expected_type, uint8_t *data, uint8_t length);
  uint8_t calculate_crc_(const uint8_t *data, uint8_t length);
};

}  // namespace wf183de_uart
}  // namespace esphome
