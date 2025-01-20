#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

#define WF183DE_I2C_ADDRESS 0x6D  // I2C 地址

namespace esphome {
namespace wf183de_i2c {

class WF183DE_I2C_Sensor : public PollingComponent, public i2c::I2CDevice {
 public:
  Sensor *temperature_sensor = new Sensor();
  Sensor *pressure_sensor = new Sensor();

  // 构造函数：传入更新间隔
  WF183DE_I2C_Sensor(uint32_t update_interval) : PollingComponent(update_interval) {}

  void setup() override {
    ESP_LOGD("WF183DE_I2C", "Sensor setup completed");
  }

  void update() override {
    float temperature = 0.0;
    float pressure = 0.0;

    // 读取温度
    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0A);  // 温度命令寄存器
    Wire.write(0x04);  // 触发温度转换命令
    if (Wire.endTransmission() != 0) {
      ESP_LOGE("WF183DE_I2C", "Failed to start temperature conversion");
      return;
    }
    delay(40);

    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0F);  // 温度数据寄存器
    Wire.endTransmission();
    Wire.requestFrom(WF183DE_I2C_ADDRESS, 2);
    if (Wire.available() == 2) {
      int16_t temp_raw = (Wire.read() << 8) | Wire.read();
      temperature = temp_raw / 10.0;
      ESP_LOGD("WF183DE_I2C", "Temperature raw: %d, converted: %.1f °C", temp_raw, temperature);
      temperature_sensor->publish_state(temperature);
    } else {
      ESP_LOGE("WF183DE_I2C", "Failed to read temperature data");
    }

    // 读取压力
    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0A);  // 压力命令寄存器
    Wire.write(0x06);  // 触发压力转换命令
    if (Wire.endTransmission() != 0) {
      ESP_LOGE("WF183DE_I2C", "Failed to start pressure conversion");
      return;
    }
    delay(40);

    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0B);  // 压力数据寄存器
    Wire.endTransmission();
    Wire.requestFrom(WF183DE_I2C_ADDRESS, 4);
    if (Wire.available() == 4) {
      uint32_t pressure_raw = (Wire.read() << 24) | (Wire.read() << 16) | (Wire.read() << 8) | Wire.read();
      pressure = pressure_raw / 1000.0;
      ESP_LOGD("WF183DE_I2C", "Pressure raw: %u, converted: %.2f kPa", pressure_raw, pressure);
      pressure_sensor->publish_state(pressure);
    } else {
      ESP_LOGE("WF183DE_I2C", "Failed to read pressure data");
    }
  }

  void set_temperature_sensor(Sensor *sensor) { temperature_sensor = sensor; }
  void set_pressure_sensor(Sensor *sensor) { pressure_sensor = sensor; }
};

}  // namespace wf183de_i2c
}  // namespace esphome
