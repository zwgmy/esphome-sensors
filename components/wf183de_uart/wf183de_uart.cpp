#include "wf183de_uart.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wf183de_uart {

static const char *TAG = "wf183de_uart";
static const uint8_t CMD_CAL_T = 0x0E;
static const uint8_t CMD_CAL_T1 = 0x27;
static const uint8_t CMD_CAL_P1 = 0x0D;
static const uint8_t CMD_CAL_P2 = 0x26;
static const uint8_t CMD_CAL_P3 = 0x3C;

void WF183DE_UART::setup() {
  ESP_LOGCONFIG(TAG, "Setting up WF183DE UART...");
}

void WF183DE_UART::update() {
  if (this->temperature_sensor_ != nullptr) {
    this->write_command_(CMD_CAL_T);
    uint8_t temp_data[2];
    if (this->read_response_(0x0A, temp_data, 2)) {
      int16_t temp = (temp_data[0] << 8) | temp_data[1];
      float temperature = temp / 100.0;
      this->temperature_sensor_->publish_state(temperature);
    }
  }

  if (this->pressure_sensor_ != nullptr) {
    this->write_command_(CMD_CAL_P1);
    uint8_t pressure_data[4];
    if (this->read_response_(0x09, pressure_data, 4)) {
      uint32_t pressure = (pressure_data[0] << 24) | (pressure_data[1] << 16) | (pressure_data[2] << 8) | pressure_data[3];
      float pressure_hpa = pressure / 100.0;
      this->pressure_sensor_->publish_state(pressure_hpa);
    }
  }
}

void WF183DE_UART::dump_config() {
  ESP_LOGCONFIG(TAG, "WF183DE UART:");
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("  ", "Pressure", this->pressure_sensor_);
}

void WF183DE_UART::write_command_(uint8_t command) {
  uint8_t frame[4];
  frame[0] = 0x55;
  frame[1] = 1;
  frame[2] = command;
  frame[3] = this->calculate_crc_(frame, 3);
  this->write_array(frame, 4);
}

bool WF183DE_UART::read_response_(uint8_t expected_type, uint8_t *data, uint8_t length) {
  uint8_t response[6 + length];
  if (this->available() < 6 + length) {
    return false;
  }
  this->read_array(response, 6 + length);
  if (response[0] != 0xAA || response[2] != expected_type || response[1] != length) {
    return false;
  }
  uint8_t crc = this->calculate_crc_(response, 5 + length);
  return crc == response[5 + length];
}

uint8_t WF183DE_UART::calculate_crc_(const uint8_t *data, uint8_t length) {
  uint8_t crc = 0;
  for (uint8_t i = 0; i < length; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x01) {
        crc = (crc >> 1) ^ 0x8C;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

}  // namespace wf183de_uart
}  // namespace esphome
