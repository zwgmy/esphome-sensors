import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c, sensor
from esphome.const import UNIT_CELSIUS, UNIT_HECTOPASCAL, ICON_THERMOMETER, ICON_GAUGE

DEPENDENCIES = ['i2c']

wf183de_i2c_ns = cg.esphome_ns.namespace('wf183de_i2c')
WF183DE_I2C_Sensor = wf183de_i2c_ns.class_('WF183DE_I2C_Sensor', cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = sensor.sensor_schema(WF183DE_I2C_Sensor).extend({
    cv.GenerateID(): cv.declare_id(WF183DE_I2C_Sensor),
    cv.Optional('temperature'): sensor.sensor_schema(UNIT_CELSIUS, ICON_THERMOMETER, 1),
    cv.Optional('pressure'): sensor.sensor_schema(UNIT_HECTOPASCAL, ICON_GAUGE, 1),
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x6D))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)

    if 'temperature' in config:
        sens = yield sensor.new_sensor(config['temperature'])
        cg.add(var.set_temperature_sensor(sens))

    if 'pressure' in config:
        sens = yield sensor.new_sensor(config['pressure'])
        cg.add(var.set_pressure_sensor(sens))
