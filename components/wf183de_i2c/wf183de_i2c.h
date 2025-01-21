#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "Wire.h"  // 确保包含 Wire.h

#define WF183DE_I2C_ADDRESS 0x6D  // I2C 地址

namespace esphome {
namespace wf183de_i2c {

class WF183DE_I2C_Sensor : public PollingComponent, public i2c::I2CDevice {
 public:
  sensor::Sensor *temperature_sensor;
  sensor::Sensor *pressure_sensor;

  // 构造函数：传入更新间隔
  WF183DE_I2C_Sensor(uint32_t update_interval);

  void setup() override;
  void update() override;

  void set_temperature_sensor(sensor::Sensor *sensor) { temperature_sensor = sensor; }
  void set_pressure_sensor(sensor::Sensor *sensor) { pressure_sensor = sensor; }
};

}  // namespace wf183de_i2c
}  // namespace esphome
