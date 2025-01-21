import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import one_wire, sensor
from esphome.const import (
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_PRESSURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_HECTOPASCAL,
    CONF_ID,
    CONF_ADDRESS,
    CONF_UPDATE_INTERVAL
)

# 定义命名空间
wf183de_owi_ns = cg.esphome_ns.namespace("wf183de_owi")

# 定义传感器类
WF183DE_OWI_Sensor = wf183de_owi_ns.class_(
    "WF183DE_OWI_Sensor",
    cg.PollingComponent,
    one_wire.OneWireDevice,
)

# 配置模式
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(WF183DE_OWI_Sensor),
            cv.Required(CONF_ADDRESS): cv.hex_int,
            cv.Optional(CONF_UPDATE_INTERVAL, default="60s"): cv.update_interval,
            cv.Optional("temperature"): sensor.sensor_schema(
                WF183DE_OWI_Sensor,
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("pressure"): sensor.sensor_schema(
                WF183DE_OWI_Sensor,
                unit_of_measurement=UNIT_HECTOPASCAL,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_PRESSURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(one_wire.one_wire_device_schema())
    .extend(cv.polling_component_schema("60s"))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_ADDRESS], config[CONF_UPDATE_INTERVAL])
    await cg.register_component(var, config)
    await one_wire.register_one_wire_device(var, config)

    if "temperature" in config:
        temp_sensor = await sensor.new_sensor(config["temperature"])
        cg.add(var.set_temperature_sensor(temp_sensor))

    if "pressure" in config:
        pressure_sensor = await sensor.new_sensor(config["pressure"])
        cg.add(var.set_pressure_sensor(pressure_sensor))
