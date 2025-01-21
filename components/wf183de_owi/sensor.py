import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, dallas
from esphome.const import UNIT_CELSIUS, UNIT_HECTOPASCAL, ICON_THERMOMETER, ICON_GAUGE, CONF_ID, CONF_PIN, CONF_ADDRESS

DEPENDENCIES = ['dallas']

wf183de_owi_ns = cg.esphome_ns.namespace('wf183de_owi')
WF183DE_OWI_Sensor = wf183de_owi_ns.class_('WF183DE_OWI_Sensor', cg.PollingComponent, dallas.DallasComponent)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(WF183DE_OWI_Sensor),
    cv.Required(CONF_PIN): pins.gpio_input_pin_schema,
    cv.Required(CONF_ADDRESS): cv.hex_uint64,
    cv.Optional('temperature'): sensor.sensor_schema(unit_of_measurement=UNIT_CELSIUS, icon=ICON_THERMOMETER, accuracy_decimals=1),
    cv.Optional('pressure'): sensor.sensor_schema(unit_of_measurement=UNIT_HECTOPASCAL, icon=ICON_GAUGE, accuracy_decimals=1),
    cv.Optional('update_interval', default="60s"): cv.update_interval,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    pin = yield cg.gpio_pin_expression(config[CONF_PIN])
    address = config[CONF_ADDRESS]
    var = cg.new_Pvariable(config[CONF_ID], config['update_interval'], pin, address)
    yield cg.register_component(var, config)
    yield dallas.register_dallas_component(var, config)

    if 'temperature' in config:
        sens = yield sensor.new_sensor(config['temperature'])
        cg.add(var.set_temperature_sensor(sens))

    if 'pressure' in config:
        sens = yield sensor.new_sensor(config['pressure'])
        cg.add(var.set_pressure_sensor(sens))
