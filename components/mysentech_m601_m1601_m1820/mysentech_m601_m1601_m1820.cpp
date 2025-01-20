#include "mysentech_m601_m1601_m1820.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mysentech_m601_m1601_m1820 {

static const char *const TAG = "mysentech.m601_m1601_m1820.temp.sensor";

static const uint8_t MYSENTECH_MODEL_M601_M1601_M1820 = 0x28;
static const uint8_t MYSENTECH_COMMAND_START_CONVERSION = 0x44;
static const uint8_t MYSENTECH_COMMAND_READ_SCRATCH_PAD = 0xBE;
static const uint8_t MYSENTECH_COMMAND_WRITE_SCRATCH_PAD = 0x4E;
static const uint8_t MYSENTECH_COMMAND_COPY_SCRATCH_PAD = 0x48;

uint16_t MysentechTemperatureSensor::millis_to_wait_for_conversion_() const {
  switch (this->repeatability_) {
    case CfgRepeatability::Low:
      return 5;
    case CfgRepeatability::Medium:
      return 7;
    case CfgRepeatability::High:
    default:
      return 12;
  }
}

void MysentechTemperatureSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "Mysentech Temperature Sensor:");
  if (this->address_ == 0) {
    ESP_LOGW(TAG, "  Unable to select an address");
    return;
  }
  LOG_ONE_WIRE_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Repeatability: %u bits", static_cast<uint8_t>(this->repeatability_));
  LOG_UPDATE_INTERVAL(this);
}

void MysentechTemperatureSensor::update() {
  if (this->address_ == 0)
    return;

  this->status_clear_warning();

  this->send_command_(MYSENTECH_COMMAND_START_CONVERSION);

  this->set_timeout(this->get_address_name(), this->millis_to_wait_for_conversion_(), [this] {
    if (!this->read_scratch_pad_() || !this->check_scratch_pad_()) {
      this->publish_state(NAN);
      return;
    }

    float tempc = this->get_temp_c_();
    ESP_LOGD(TAG, "'%s': Got Temperature=%.2f°C", this->get_name().c_str(), tempc);
    this->publish_state(tempc);
  });
}

void IRAM_ATTR MysentechTemperatureSensor::read_scratch_pad_int_() {
  for (uint8_t &i : this->scratch_pad_) {
    i = this->bus_->read8();
  }
}

bool MysentechTemperatureSensor::read_scratch_pad_() {
  bool success;
  {
    InterruptLock lock;
    success = this->send_command_(MYSENTECH_COMMAND_READ_SCRATCH_PAD);
    if (success)
      this->read_scratch_pad_int_();
  }
  if (!success) {
    ESP_LOGW(TAG, "'%s' - reading scratch pad failed bus reset", this->get_name().c_str());
    this->status_set_warning("bus reset failed");
  }
  return success;
}

void MysentechTemperatureSensor::setup() {
  ESP_LOGCONFIG(TAG, "setting up Mysentech temperature sensor...");
  if (!this->check_address_())
    return;
  if (!this->read_scratch_pad_())
    return;
  if (!this->check_scratch_pad_())
    return;

  uint8_t cfg {this->scratch_pad_[6]};

	cfg &= ~0x03;
  cfg |= static_cast<uint8_t>(this->repeatability_);
	//cfg |= std::to_underlying(this->repeatability_);

	cfg &= ~0x1C;
  cfg |= static_cast<uint8_t>(CfgMps::Single);
	//cfg |= std::to_underlying(CfgMps::Single);	

  cfg &= ~0x80;
  cfg |= static_cast<uint8_t>(CfgAlarm::Disable);
	//cfg |= std::to_underlying(CfgAlarm::Disable);

  if (this->scratch_pad_[6] == cfg)
    return;
  this->scratch_pad_[6] = cfg;

  {
    InterruptLock lock;
    if (this->send_command_(MYSENTECH_COMMAND_WRITE_SCRATCH_PAD)) {
      this->bus_->write8(this->scratch_pad_[2]);  // high alarm temp
      this->bus_->write8(this->scratch_pad_[3]);  // low alarm temp
      this->bus_->write8(this->scratch_pad_[4]);  // resolution
    }

    // write value to EEPROM
    this->send_command_(MYSENTECH_COMMAND_COPY_SCRATCH_PAD);
  }
}

bool MysentechTemperatureSensor::check_scratch_pad_() {
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

float MysentechTemperatureSensor::get_temp_c_() {
  int16_t temp = static_cast<int16_t>(static_cast<uint16_t>((this->scratch_pad_[1] << 8) | this->scratch_pad_[0]));
  return 40.0f + (temp / 256.0f);
}

}  // namespace mysentech_m601_m1601_m1820
}  // namespace esphome
