import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
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

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        T12TemperatureSensor,
        unit_of_measurement=UNIT_CELSIUS,
        icon=ICON_THERMOMETER,
        accuracy_decimals=1,
        state_class=STATE_CLASS_MEASUREMENT,
        device_class=DEVICE_CLASS_TEMPERATURE,
    )
    .extend(cv.polling_component_schema("60s"))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
