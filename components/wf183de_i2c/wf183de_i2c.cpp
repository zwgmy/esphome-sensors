#include "wf183de_i2c_sensor.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wf183de {
void WF183DESensor::setup() {
    ESP_LOGD("WF183DE", "WF183DE Sensor setup completed");
}

void WF183DESensor::update() {
    // 读取温度数据
    Wire.beginTransmission(WF183DE_I2C_ADDRESS);  // 传感器 I2C 地址
    Wire.write(0x0A);              // 温度命令寄存器
    Wire.write(0x04);              // 触发温度转换命令
    if (Wire.endTransmission() != 0) {
        ESP_LOGE("WF183DE", "Failed to start temperature conversion");
        return;
    }
    delay(40);

    // 读取温度值
    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0F);  // 温度数据寄存器（高字节）
    if (Wire.endTransmission() != 0 || Wire.requestFrom(0x6D, 2) != 2) {
        ESP_LOGE("WF183DE", "Failed to read temperature data");
        return;
    }
    int16_t temp_raw = (Wire.read() << 8) | Wire.read();  // 组合高低字节
    float temperature = temp_raw / 10.0;  // 根据数据手册，将原始值除以10
    ESP_LOGD("WF183DE", "Temperature raw: %d, converted: %.1f °C", temp_raw, temperature);
    temperature_sensor->publish_state(temperature);

    // 读取压力数据
    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0A);              // 压力命令寄存器
    Wire.write(0x06);              // 触发压力转换命令
    if (Wire.endTransmission() != 0) {
        ESP_LOGE("WF183DE", "Failed to start pressure conversion");
        return;
    }
    delay(40);

    // 读取压力值
    Wire.beginTransmission(WF183DE_I2C_ADDRESS);
    Wire.write(0x0B);  // 压力数据寄存器（高字节）
    if (Wire.endTransmission() != 0 || Wire.requestFrom(0x6D, 4) != 4) {
        ESP_LOGE("WF183DE", "Failed to read pressure data");
        return;
    }
    uint32_t pressure_raw = (Wire.read() << 24) | (Wire.read() << 16) | (Wire.read() << 8) | Wire.read();  // 组合四个字节
    float pressure = pressure_raw / 1000.0;  // 将原始值除以1000转换为kPa
    ESP_LOGD("WF183DE", "Pressure raw: %u, converted: %.2f kPa", pressure_raw, pressure);
    pressure_sensor->publish_state(pressure);
}



}  // namespace bmp280_i2c
}  // namespace esphome
