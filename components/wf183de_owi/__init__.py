import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, one_wire
from esphome.const import UNIT_CELSIUS, UNIT_HECTOPASCAL, ICON_THERMOMETER, ICON_GAUGE, CONF_ID, CONF_ADDRESS

# 常量定义
CONF_ONE_WIRE_ID = "one_wire_id"

# 依赖 OneWire 总线
DEPENDENCIES = ['one_wire']

# 创建命名空间
wf183de_owi_ns = cg.esphome_ns.namespace('wf183de_owi')

# 定义 WF183DE_OWI_Sensor 类
WF183DE_OWI_Sensor = wf183de_owi_ns.class_(
    'WF183DE_OWI_Sensor', cg.PollingComponent, one_wire.OneWireDevice
)

# 定义配置模式
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(WF183DE_OWI_Sensor),
    cv.Required(CONF_ONE_WIRE_ID): cv.use_id(one_wire.OneWireBus),
    cv.Required(CONF_ADDRESS): cv.hex_int,
    cv.Optional('temperature'): sensor.sensor_schema(unit_of_measurement=UNIT_CELSIUS, icon=ICON_THERMOMETER, accuracy_decimals=1),
    cv.Optional('pressure'): sensor.sensor_schema(unit_of_measurement=UNIT_HECTOPASCAL, icon=ICON_GAUGE, accuracy_decimals=1),
    cv.Optional('update_interval', default="60s"): cv.update_interval,
}).extend(cv.COMPONENT_SCHEMA)

# to_code 函数，用于将配置转换为 C++ 代码
async def to_code(config):
    # 获取 OneWire 总线实例
    parent = yield cg.get_variable(config[CONF_ONE_WIRE_ID])

    # 创建 WF183DE_OWI_Sensor 实例
    var = cg.new_Pvariable(config[CONF_ID], config['update_interval'], config[CONF_ADDRESS])
    
    # 注册组件
    yield cg.register_component(var, config)
    
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

