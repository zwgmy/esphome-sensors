#include "wf183de_owi.h"
#include "esphome/core/log.h"
#include "Arduino.h"  // 确保包含 Arduino.h 以使用 delay 函数

namespace esphome {
namespace wf183de_owi {

static const char *TAG = "wf183de_owi";

WF183DE_OWI_Sensor::WF183DE_OWI_Sensor(uint32_t update_interval, InternalGPIOPin *pin)
    : PollingComponent(update_interval), OneWire(pin) {}

void WF183DE_OWI_Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up WF183DE OWI Sensor...");
}

void WF183DE_OWI_Sensor::update() {
  float temperature = 0.0;
  float pressure = 0.0;

  // 发送请求温度和压力的命令
  if (!reset()) {  // 重置总线，检查传感器连接
    ESP_LOGW(TAG, "No response from OWI sensor");
    return;
  }

  // 假设传感器支持的命令格式为 0x44（触发温度转换），读取温度和压力
  write_byte(0x44);  // 替换为 WF183DE 的触发数据读取命令
  delay(750);  // 等待转换完成（调整延时以符合 WF183DE 的要求）

  // 读取数据，假设返回数据格式为：2字节温度 + 2字节压力
  uint8_t data[4];
  reset();
  write_byte(0xBE);  // 替换为 WF183DE 的数据读取命令
  for (int i = 0; i < 4; i++) {
    data[i] = read_byte();
  }

  // 将读取到的数据转换为温度和压力
  int16_t temp_raw = (data[1] << 8) | data[0];
  int16_t pressure_raw = (data[3] << 8) | data[2];
  float temperature = temp_raw / 10.0;  // 假设温度数据需要除以10
  float pressure = pressure_raw / 100.0;  // 假设压力数据需要除以100

  ESP_LOGD(TAG, "Temperature raw: %d, converted: %.1f °C", temp_raw, temperature);
  ESP_LOGD(TAG, "Pressure raw: %d, converted: %.2f kPa", pressure_raw, pressure);

  // 发布传感器数据
  temperature_sensor->publish_state(temperature);
  pressure_sensor->publish_state(pressure);
}

}  // namespace wf183de_owi
}  // namespace esphome
