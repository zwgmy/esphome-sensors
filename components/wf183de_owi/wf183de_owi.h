#pragma once

#include "esphome/core/component.h"
#include "esphome/components/dallas/dallas.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace wf183de_owi {

class WF183DE_OWI_Sensor : public PollingComponent, public dallas::DallasComponent {
 public:
  sensor::Sensor *temperature_sensor;
  sensor::Sensor *pressure_sensor;

  // 构造函数：传入更新间隔、One-Wire引脚和传感器地址
  WF183DE_OWI_Sensor(uint32_t update_interval, InternalGPIOPin *pin, uint64_t address);

  void setup() override;
  void update() override;

  void set_temperature_sensor(sensor::Sensor *sensor) { temperature_sensor = sensor; }
  void set_pressure_sensor(sensor::Sensor *sensor) { pressure_sensor = sensor; }

 private:
  uint64_t address_;
};

}  // namespace wf183de_owi
}  // namespace esphome
