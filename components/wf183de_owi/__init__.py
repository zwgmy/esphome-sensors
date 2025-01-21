import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, one_wire
from esphome.const import UNIT_CELSIUS, UNIT_HECTOPASCAL, ICON_THERMOMETER, ICON_GAUGE, CONF_ID, CONF_ADDRESS

CONF_ONE_WIRE_ID = "one_wire_id"

DEPENDENCIES = ['one_wire']

wf183de_owi_ns = cg.esphome_ns.namespace('wf183de_owi')
WF183DE_OWI_Sensor = wf183de_owi_ns.class_(
    'WF183DE_OWI_Sensor', cg.PollingComponent, one_wire.OneWireDevice
)

# 定义配置模式
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(WF183DE_OWI_Sensor),
    cv.Required(CONF_ONE_WIRE_ID): cv.use_id(one_wire.OneWireBus),  # 正确引用 OneWire 总线
    cv.Required(CONF_ADDRESS): cv.hex_int,
    cv.Optional('temperature'): sensor.sensor_schema(unit_of_measurement=UNIT_CELSIUS, icon=ICON_THERMOMETER, accuracy_decimals=1),
    cv.Optional('pressure'): sensor.sensor_schema(unit_of_measurement=UNIT_HECTOPASCAL, icon=ICON_GAUGE, accuracy_decimals=1),
    cv.Optional('update_interval', default="60s"): cv.update_interval,
}).extend(cv.COMPONENT_SCHEMA)

# 转换配置并注册组件
async def to_code(config):
    # 获取 OneWire 总线实例
    parent = yield cg.get_variable(config[CONF_ONE_WIRE_ID])  # 获取 OneWire 总线的实例

    # 创建传感器对象
    var = cg.new_Pvariable(config[CONF_ID], config['update_interval'], config[CONF_ADDRESS])
    yield cg.register_component(var, config)  # 注册组件

    # 注册 OneWire 设备
    yield one_wire.register_one_wire_device(var, parent)

    # 如果配置中有温度传感器
    if 'temperature' in config:
        sens = yield sensor.new_sensor(config['temperature'])  # 创建温度传感器
        cg.add(var.set_temperature_sensor(sens))  # 将温度传感器与设备绑定

    # 如果配置中有压力传感器
    if 'pressure' in config:
        sens = yield sensor.new_sensor(config['pressure'])  # 创建压力传感器
        cg.add(var.set_pressure_sensor(sens))  # 将压力传感器与设备绑定
