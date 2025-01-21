import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import one_wire, sensor
from esphome.const import (
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)
CONF_REPEATABILITY = "repeatability"
mysentech_m601_m1601_m1820_ns = cg.esphome_ns.namespace("mysentech_m601_m1601_m1820")

MysentechTemperatureSensor = mysentech_m601_m1601_m1820_ns.class_(
    "MysentechTemperatureSensor",
    cg.PollingComponent,
    sensor.Sensor,
    one_wire.OneWireDevice,
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        MysentechTemperatureSensor,
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.Optional(CONF_REPEATABILITY, default=2): cv.int_range(min=0, max=2),
        }
    )
    .extend(one_wire.one_wire_device_schema())
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await one_wire.register_one_wire_device(var, config)

    cg.add(var.set_repeatability(config[CONF_REPEATABILITY]))
