#include "esphome.h"

class T12TemperatureSensor : public PollingComponent, public Sensor {
 public:
  T12TemperatureSensor() : PollingComponent(0) {} // 不使用自动轮询，手动更新

  // 电压-温度对应表 (电压单位: mV, 温度单位: °C)
  const float voltage_points[11] = {0.0, 0.25, 1.0, 2.0, 3.0, 4.0, 5.0, 5.5, 6.32, 7.0, 8.0};
  const float temperature_points[11] = {0.0, 38.0, 84.0, 134.0, 176.0, 218.0, 262.0, 280.0, 334.0, 352.0, 380.0};
  const int table_size = 11;

  void setup() override {
    // 初始化ADC引脚
    pinMode(4, INPUT); // GPIO4用于ADC读取
    // 设置ADC衰减 (0dB = 3.3V满量程)
    analogSetPinAttenuation(4, ADC_0db);
  }

  void update() override {
    // 读取ADC值
    int adc_value = analogRead(4);
    // 转换为电压 (mV)
    float voltage_mv = (adc_value / 4095.0) * 3300.0;
    
    // 减去运放偏移电压 (需要校准)
    voltage_mv -= offset_voltage_mv;
    
    // 根据GS8592的放大倍数计算实际热电偶电压
    float thermocouple_voltage_mv = voltage_mv / amplification_factor;
    
    // 将电压转换为温度 (使用查表法)
    float temperature = lookup_temperature(thermocouple_voltage_mv);
    
    // 发布温度值
    publish_state(temperature);
  }

  void set_offset_voltage(float offset) {
    offset_voltage_mv = offset;
  }

  void set_amplification_factor(float factor) {
    amplification_factor = factor;
  }

 private:
  float offset_voltage_mv = 100.0; // 默认偏移电压，需要实际校准
  float amplification_factor = 101.0; // GS8592放大倍数，需要根据实际电路调整

  float lookup_temperature(float voltage_mv) {
    // 检查电压是否超出范围
    if (voltage_mv <= voltage_points[0]) {
      return temperature_points[0];
    }
    if (voltage_mv >= voltage_points[table_size - 1]) {
      return temperature_points[table_size - 1];
    }
    
    // 查找电压所在的区间
    int index = 0;
    for (int i = 0; i < table_size - 1; i++) {
      if (voltage_mv >= voltage_points[i] && voltage_mv <= voltage_points[i + 1]) {
        index = i;
        break;
      }
    }
    
    // 线性插值计算温度
    float voltage_low = voltage_points[index];
    float voltage_high = voltage_points[index + 1];
    float temp_low = temperature_points[index];
    float temp_high = temperature_points[index + 1];
    
    // 计算插值比例
    float ratio = (voltage_mv - voltage_low) / (voltage_high - voltage_low);
    
    // 计算插值温度
    return temp_low + ratio * (temp_high - temp_low);
  }
};