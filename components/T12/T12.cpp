#include "t12_temperature_sensor.h"

namespace esphome {
namespace t12_temperature_sensor {

// 电压-温度对应表 (电压单位: mV, 温度单位: °C)
const float T12TemperatureSensor::voltage_points[11] = {0.0, 0.25, 1.0, 2.0, 3.0, 4.0, 5.0, 5.5, 6.32, 7.0, 8.0};
const float T12TemperatureSensor::temperature_points[11] = {0.0, 38.0, 84.0, 134.0, 176.0, 218.0, 262.0, 280.0, 334.0, 352.0, 380.0};
const int T12TemperatureSensor::table_size = 11;

T12TemperatureSensor::T12TemperatureSensor() : offset_voltage_mv_(100.0), amplification_factor_(200.0), last_heating_time_(0) {}

void T12TemperatureSensor::setup() {
  pinMode(4, INPUT); // GPIO4用于ADC读取
  analogSetPinAttenuation(4, ADC_0db);
}

void T12TemperatureSensor::update() {
  // 检查是否在加热，如果是则更新最后加热时间
  auto climate = static_cast<climate::Climate*>(id(t12_climate).get_component());
  if (climate && climate->mode == climate::CLIMATE_MODE_HEAT) {
    last_heating_time_ = millis();
  }
  
  // 如果不在加热状态，且距离上次加热已超过50ms，则进行测量
  if (millis() - last_heating_time_ > 50) {
    int adc_value = analogRead(4);
    float voltage_mv = (adc_value / 4095.0) * 3300.0;
    voltage_mv -= offset_voltage_mv_;
    float thermocouple_voltage_mv = voltage_mv / amplification_factor_;
    float temperature = lookup_temperature(thermocouple_voltage_mv);
    publish_state(temperature);
  }
}

void T12TemperatureSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "T12 Temperature Sensor:");
  ESP_LOGCONFIG(TAG, "  Offset Voltage: %.2f mV", offset_voltage_mv_);
  ESP_LOGCONFIG(TAG, "  Amplification Factor: %.2f", amplification_factor_);
}

float T12TemperatureSensor::lookup_temperature(float voltage_mv) {
  if (voltage_mv <= voltage_points[0]) return temperature_points[0];
  if (voltage_mv >= voltage_points[table_size - 1]) return temperature_points[table_size - 1];
  
  int index = 0;
  for (int i = 0; i < table_size - 1; i++) {
    if (voltage_mv >= voltage_points[i] && voltage_mv <= voltage_points[i + 1]) {
      index = i;
      break;
    }
  }
  
  float voltage_low = voltage_points[index];
  float voltage_high = voltage_points[index + 1];
  float temp_low = temperature_points[index];
  float temp_high = temperature_points[index + 1];
  
  float ratio = (voltage_mv - voltage_low) / (voltage_high - voltage_low);
  return temp_low + ratio * (temp_high - temp_low);
}

}  // namespace t12_temperature_sensor
}  // namespace esphome
