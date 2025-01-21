#include "wf183de_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wf183de_i2c {

static const char *TAG = "wf183de_i2c";

void WF183DE_I2C_Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up WF183DE I2C Sensor...");
}

void WF183DE_I2C_Sensor::update() {
  float temperature = 0.0;
  float pressure = 0.0;

  // 读取温度
  Wire.beginTransmission(WF183DE_I2C_ADDRESS);
  Wire.write(0x0A);  // 温度命令寄存器
  Wire.write(0x04);  // 触发温度转换命令
  if (Wire.endTransmission() != 0) {
    ESP_LOGE(TAG, "Failed to start temperature conversion");
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
    ESP_LOGD(TAG, "Temperature raw: %d, converted: %.1f °C", temp_raw, temperature);
    temperature_sensor->publish_state(temperature);
  } else {
    ESP_LOGE(TAG, "Failed to read temperature data");
  }

  // 读取压力
  Wire.beginTransmission(WF183DE_I2C_ADDRESS);
  Wire.write(0x0A);  // 压力命令寄存器
  Wire.write(0x06);  // 触发压力转换命令
  if (Wire.endTransmission() != 0) {
    ESP_LOGE(TAG, "Failed to start pressure conversion");
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
    ESP_LOGD(TAG, "Pressure raw: %u, converted: %.2f kPa", pressure_raw, pressure);
    pressure_sensor->publish_state(pressure);
  } else {
    ESP_LOGE(TAG, "Failed to read pressure data");
  }
}

}  // namespace wf183de_i2c
}  // namespace esphome
