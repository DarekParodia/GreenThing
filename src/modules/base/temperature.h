#pragma once
#include "core/mqtt.h"
#include "modules/base/module.h"

#include <string>

namespace modules {
    class Temperature : public virtual Module {
        public:
            Temperature() :
                Module() {};
            inline double getTemperature() {
                return temperature;
            }

        protected:
            double temperature = 0.0; // Current temperature value in celsius
#ifdef USE_MQTT
            core::mqtt::EntityMeta    mqtt_meta   = { Module::getName() + " Temperature", "°C", "temperature" };
            core::mqtt::SensorNumber *mqtt_sensor = new core::mqtt::SensorNumber(Module::getName() + "_temperature", 10000, mqtt_meta);
#endif
    };
} // namespace modules