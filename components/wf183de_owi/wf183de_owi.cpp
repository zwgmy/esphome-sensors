#include "wf183de_owi.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wf183de_owi {

static const char *const TAG = "wf183de_owi.sensor";

static const uint8_t WF183DE_MODEL = 0x28;
static const uint8_t WF183DE_COMMAND_START_CONVERSION = 0x44;
static const uint8_t WF183DE_COMMAND_READ_SCRATCH_PAD = 0xBE;
static const uint8_t WF183DE_COMMAND_WRITE_SCRATCH_PAD = 0x4E;
static const uint8_t WF183DE_COMMAND_COPY_SCRATCH_PAD = 0x48;

WF183DE_OWI_Sensor::WF183DE_OWI_Sensor(uint64_t address, uint32_t update_interval)
    : PollingComponent(update_interval), one_wire::OneWireDevice() {
  this->address_ = address;
}

uint16_t WF183DE_OWI_Sensor::millis_to_wait_for_conversion_() const {
  // 根据需要调整转换时间
  return 750;  // 假设750ms是转换时间
}

void WF183DE_OWI_Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "WF183DE OWI Sensor:");
  if (this->address_ == 0) {
    ESP_LOGW(TAG, "  Unable to select an address");
    return;
  }
  LOG_ONE_WIRE_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
}

void WF183DE_OWI_Sensor::update() {
  if (this->address_ == 0)
    return;

  this->status_clear_warning();

  this->send_command_(WF183DE_COMMAND_START_CONVERSION);

  this->set_timeout(this->get_address_name(), this->millis_to_wait_for_conversion_(), [this] {
    if (!this->read_scratch_pad_() || !this->check_scratch_pad_()) {
      if (this->temperature_sensor_ != nullptr) {
        this->temperature_sensor_->publish_state(NAN);
      }
      if (this->pressure_sensor_ != nullptr) {
        this->pressure_sensor_->publish_state(NAN);
      }
      return;
    }

    if (this->temperature_sensor_ != nullptr) {
      float tempc = this->get_temp_c_();
      ESP_LOGD(TAG, "'%s': Got Temperature=%.2f°C", this->get_name().c_str(), tempc);
      this->temperature_sensor_->publish_state(tempc);
    }

    if (this->pressure_sensor_ != nullptr) {
      float pressure = this->get_pressure_hpa_();
      ESP_LOGD(TAG, "'%s': Got Pressure=%.2fhPa", this->get_name().c_str(), pressure);
      this->pressure_sensor_->publish_state(pressure);
    }
  });
}

void WF183DE_OWI_Sensor::read_scratch_pad_int_() {
  for (uint8_t &i : this->scratch_pad_) {
    i = this->bus_->read8();
  }
}

bool WF183DE_OWI_Sensor::read_scratch_pad_() {
  bool success;
  {
    InterruptLock lock;
    success = this->send_command_(WF183DE_COMMAND_READ_SCRATCH_PAD);
    if (success)
      this->read_scratch_pad_int_();
  }
  if (!success) {
    ESP_LOGW(TAG, "'%s' - reading scratch pad failed bus reset", this->get_name().c_str());
    this->status_set_warning("bus reset failed");
  }
  return success;
}

void WF183DE_OWI_Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up WF183DE OWI sensor...");
  if (!this->check_address_())
    return;
  if (!this->read_scratch_pad_())
    return;
  if (!this->check_scratch_pad_())
    return;

  uint8_t cfg = this->scratch_pad_[6];

  // 根据需要配置传感器

  if (this->scratch_pad_[6] == cfg)
    return;
  this->scratch_pad_[6] = cfg;

  {
    InterruptLock lock;
    if (this->send_command_(WF183DE_COMMAND_WRITE_SCRATCH_PAD)) {
      this->bus_->write8(this->scratch_pad_[2]);  // high alarm temp
      this->bus_->write8(this->scratch_pad_[3]);  // low alarm temp
      this->bus_->write8(this->scratch_pad_[4]);  // resolution
    }

    // write value to EEPROM
    this->send_command_(WF183DE_COMMAND_COPY_SCRATCH_PAD);
  }
}

bool WF183DE_OWI_Sensor::check_scratch_pad_() {
  bool chksum_validity = (crc8(this->scratch_pad_, 8) == this->scratch_pad_[8]);

#ifdef ESPHOME_LOG_LEVEL_VERY_VERBOSE
  ESP_LOGVV(TAG, "Scratch pad: %02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X (%02X)", this->scratch_pad_[0],
            this->scratch_pad_[1], this->scratch_pad_[2], this->scratch_pad_[3], this->scratch_pad_[4],
            this->scratch_pad_[5], this->scratch_pad_[6], this->scratch_pad_[7], this->scratch_pad_[8],
            crc8(this->scratch_pad_, 8));
#endif
  if (!chksum_validity) {
    ESP_LOGW(TAG, "'%s' - Scratch pad checksum invalid!", this->get_name().c_str());
    this->status_set_warning("scratch pad checksum invalid");
  }
  return chksum_validity;
}

float WF183DE_OWI_Sensor::get_temp_c_() {
  int16_t temp = static_cast<int16_t>(static_cast<uint16_t>((this->scratch_pad_[1] << 8) | this->scratch_pad_[0]));
  return static_cast<float>(temp) / 16.0f;  // 假设温度数据需要除以16
}

float WF183DE_OWI_Sensor::get_pressure_hpa_() {
  int16_t pressure = static_cast<int16_t>(static_cast<uint16_t>((this->scratch_pad_[3] << 8) | this->scratch_pad_[2]));
  return static_cast<float>(pressure) / 100.0f;  // 假设压力数据需要除以100
}

}  // namespace wf183de_owi
}  // namespace esphome
