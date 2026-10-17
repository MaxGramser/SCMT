#pragma once
#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome {
namespace scmt {

class Scmt : public Component, public tessera::Plugin {
 public:
  void setup() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

#ifdef USE_SENSOR
  // The tempo sensor of plugin.yaml: the Home Assistant entity chosen for the screen (input "bpm").
  void set_bpm(sensor::Sensor *s) { bpm_ = s; }
#endif
  // The tempo in whole BPM, 0 while the sensor has no value.
  int bpm() const;

 protected:
#ifdef USE_SENSOR
  sensor::Sensor *bpm_{nullptr};
#endif
};

}  // namespace scmt
}  // namespace esphome
