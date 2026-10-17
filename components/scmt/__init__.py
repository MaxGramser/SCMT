import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, smart_display
from esphome.const import CONF_ID

DEPENDENCIES = ["smart_display"]

CONF_BPM = "bpm"

scmt_ns = cg.esphome_ns.namespace("scmt")
Scmt = scmt_ns.class_("Scmt", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Scmt),
        # The tempo the tile shows: a homeassistant sensor of plugin.yaml.
        cv.Optional(CONF_BPM): cv.use_id(sensor.Sensor),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await smart_display.register_plugin(var, __file__)
    if CONF_BPM in config:
        cg.add(var.set_bpm(await cg.get_variable(config[CONF_BPM])))
