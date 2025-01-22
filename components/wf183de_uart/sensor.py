import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import (
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_PRESSURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_HECTOPASCAL,
    CONF_ID,
    CONF_UPDATE_INTERVAL
)

DEPENDENCIES = ['uart']

CONF_WF183DE_UART_ID = 'wf183de_uart_id'
CONF_TEMPERATURE = 'temperature'
CONF_PRESSURE = 'pressure'

wf183de_uart_ns = cg.esphome_ns.namespace('wf183de_uart')
WF183DE_UART = wf183de_uart_ns.class_('WF183DE_UART', cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(WF183DE_UART),
    cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_PRESSURE): sensor.sensor_schema(
        unit_of_measurement=UNIT_HECTOPASCAL,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_PRESSURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
}).extend(cv.polling_component_schema('60s')).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_WF183DE_UART_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_TEMPERATURE in config:
        temp = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature_sensor(temp))

    if CONF_PRESSURE in config:
        pressure = await sensor.new_sensor(config[CONF_PRESSURE])
        cg.add(var.set_pressure_sensor(pressure))
