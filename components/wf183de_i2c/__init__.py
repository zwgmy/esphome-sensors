import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
#from ..bmp280_base import to_code_base, CONFIG_SCHEMA_BASE

#AUTO_LOAD = ["bmp280_base"]
CODEOWNERS = ["@zwgmy"]
DEPENDENCIES = ["i2c"]

wf183de_ns = cg.esphome_ns.namespace("wf183de")
WF183DEComponent = wf183de_ns.class_(
    "WF183DEComponent", cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = CONFIG_SCHEMA_BASE.extend(
    i2c.i2c_device_schema(default_address=0x6D)
).extend({cv.GenerateID(): cv.declare_id(WF183DEComponent)})


async def to_code(config):
    var = await to_code_base(config)
    await i2c.register_i2c_device(var, config)
