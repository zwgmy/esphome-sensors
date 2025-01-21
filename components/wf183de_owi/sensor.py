import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, one_wire
from esphome.const import UNIT_CELSIUS, UNIT_HECTOPASCAL, ICON_THERMOMETER, ICON_GAUGE, CONF_ID, CONF_ADDRESS, CONF_ONE_WIRE_ID

DEPENDENCIES = ['one_wire']

wf183de_owi_ns = cg.esphome_ns.namespace('wf183de_owi')
WF183DE_OWI_Sensor = wf183de_owi_ns.class_('WF183DE_OWI_Sensor', cg.PollingComponent, one_wire.OneWireDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(WF183DE_OWI_Sensor),
    cv.Required(CONF_ONE_WIRE_ID): cv.use_id(one_wire.OneWireBus),
    cv.Required(CONF_ADDRESS): cv.hex_int,
    cv.Optional('temperature'): sensor.sensor_schema(unit_of_measurement=UNIT_CELSIUS, icon=ICON_THERMOMETER, accuracy_decimals=1),
    cv.Optional('pressure'): sensor.sensor_schema(unit_of_measurement=UNIT_HECTOPASCAL, icon=ICON_GAUGE, accuracy_decimals=1),
    cv.Optional('update_interval', default="60s"): cv.update_interval,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    address = config[CONF_ADDRESS]
    parent = yield cg.get_variable(config[CONF_ONE_WIRE_ID])
    var = cg.new_Pvariable(config[CONF_ID], config['update_interval'], address)
    yield cg.register_component(var, config)
    yield one_wire.register_one_wire_device(var, parent)

    if 'temperature' in config:
        sens = yield sensor.new_sensor(config['temperature'])
        cg.add(var.set_temperature_sensor(sens))

    if 'pressure' in config:
        sens = yield sensor.new_sensor(config['pressure'])
        cg.add(var.set_pressure_sensor(sens))
