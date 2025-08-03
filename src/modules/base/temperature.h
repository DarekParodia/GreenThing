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
            core::mqtt::mqtt_data<double> *mqtt_data = new core::mqtt::mqtt_data<double>(Module::getName() + "/temperature");
#endif
    };
} // namespace modules