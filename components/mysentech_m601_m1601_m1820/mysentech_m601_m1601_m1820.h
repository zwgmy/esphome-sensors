#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/one_wire/one_wire.h"

namespace esphome {
namespace mysentech_m601_m1601_m1820 {

class MysentechTemperatureSensor : public PollingComponent, public sensor::Sensor, public one_wire::OneWireDevice {
  enum class CfgRepeatability: uint8_t{
                    //              转换时间tCONV
    Low = 0x00,     // 低可重复性     4ms
    Medium = 0x01,  // 中等可重复性   5.5ms
    High = 0x02,    // 高可重复性     10.5ms
  };

  //周期测量频率配置
  enum class CfgMps : uint8_t{
    Single = 0x00, // 单次
    Half = 0x04,   // 每秒 0.5 次
    _1 = 0x04,     // 每秒 1 次
    _2 = 0x04,     // 每秒 2 次
    _4 = 0x04,     // 每秒 4 次
    _10 = 0x04,    // 每秒 10 次
  };

  //温度报警功能
  enum class CfgAlarm : uint8_t{
    Enable = 0x80,  // 温度报警功能开启
    Disable = 0x00, // 温度报警功能关闭
  };

 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_repeatability(uint8_t repeatability) { 
    switch(repeatability){
      case 0: this->repeatability_ =   CfgRepeatability::Low;     break;
      case 1: this->repeatability_ =   CfgRepeatability::Medium;  break;
      case 2: 
      default:  this->repeatability_ = CfgRepeatability::High;    break;
    }
    }

 protected:
  CfgRepeatability repeatability_;
  uint8_t scratch_pad_[9] = {0};
  
  /// Get the number of milliseconds we have to wait for the conversion phase.
  uint16_t millis_to_wait_for_conversion_() const;
  bool read_scratch_pad_();
  void read_scratch_pad_int_();
  bool check_scratch_pad_();
  float get_temp_c_();
};

}  // namespace mysentech_m601_m1601_m1820
}  // namespace esphome
