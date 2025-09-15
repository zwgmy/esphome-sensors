import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_OFFSET,
    CONF_FACTOR,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    ICON_THERMOMETER,
)

CODEOWNERS = ["@您的用户名"]
DEPENDENCIES = []

t12_temperature_sensor_ns = cg.esphome_ns.namespace("t12_temperature_sensor")
T12TemperatureSensor = t12_temperature_sensor_ns.class_(
    "T12TemperatureSensor", sensor.Sensor, cg.PollingComponent
)

CONF_OFFSET_VOLTAGE = "offset_voltage"
CONF_AMPLIFICATION_FACTOR = "amplification_factor"

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        T12TemperatureSensor,
        unit_of_measurement=UNIT_CELSIUS,
        icon=ICON_THERMOMETER,
        accuracy_decimals=1,
        state_class=STATE_CLASS_MEASUREMENT,
        device_class=DEVICE_CLASS_TEMPERATURE,
    )
    .extend(cv.polling_component_schema("100ms"))
    .extend(
        {
            cv.Optional(CONF_OFFSET_VOLTAGE, default=100.0): cv.float_,
            cv.Optional(CONF_AMPLIFICATION_FACTOR, default=200.0): cv.float_,
        }
    )
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    
    cg.add(var.set_offset_voltage(config[CONF_OFFSET_VOLTAGE]))
    cg.add(var.set_amplification_factor(config[CONF_AMPLIFICATION_FACTOR]))
