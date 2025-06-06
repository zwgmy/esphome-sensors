#pragma once

#include "esphome/core/component.h"
#include "esphome/core/datatypes.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace bl0942 {

static const float BL0942_PREF = 596;              // taken from tasmota
static const float BL0942_UREF = 15873.35944299;   // should be 73989/1.218
static const float BL0942_IREF = 251213.46469622;  // 305978/1.218
static const float BL0942_EREF = 3304.61127328;    // Measured

struct DataPacket {
  uint8_t frame_header;
  uint24_le_t i_rms;
  uint24_le_t v_rms;
  uint24_le_t i_fast_rms;
  int24_le_t watt;
  uint24_le_t cf_cnt;
  uint16_le_t frequency;
  uint8_t reserved1;
  uint8_t status;
  uint8_t reserved2;
  uint8_t reserved3;
  uint8_t checksum;
} __attribute__((packed));

class BL0942 : public PollingComponent, public uart::UARTDevice {
 public:
  void set_voltage_sensor(sensor::Sensor *voltage_sensor) { voltage_sensor_ = voltage_sensor; }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_energy_sensor(sensor::Sensor *energy_sensor) { energy_sensor_ = energy_sensor; }
  void set_frequency_sensor(sensor::Sensor *frequency_sensor) { frequency_sensor_ = frequency_sensor; }

  // 新增 reset 参数，默认值 true（保持原行为）
 // void set_reset(bool reset) { reset_ = reset; } //*******增加zwgmy**************######

  void loop() override;

  void update() override;
  void setup() override;
  void dump_config() override;

 protected:
  sensor::Sensor *voltage_sensor_{nullptr};
  sensor::Sensor *current_sensor_{nullptr};
  // NB This may be negative as the circuits is seemingly able to measure
  // power in both directions
  sensor::Sensor *power_sensor_{nullptr};
  sensor::Sensor *energy_sensor_{nullptr};
  sensor::Sensor *frequency_sensor_{nullptr};

//  bool reset_{true};  // 默认 true，但允许外部修改***************增加zwgmy**************###########  

  // Divide by this to turn into Watt
  float power_reference_ = BL0942_PREF;
  // Divide by this to turn into Volt
  float voltage_reference_ = BL0942_UREF;
  // Divide by this to turn into Ampere
  float current_reference_ = BL0942_IREF;
  // Divide by this to turn into kWh
  float energy_reference_ = BL0942_EREF;

  DataPacket buffer;
  uint8_t inpos=0xFF;
  uint8_t checksum;
  uint8_t pubPhase=0xFF;
  bool needUpdate=false;

};
}  // namespace bl0942
}  // namespace esphome
