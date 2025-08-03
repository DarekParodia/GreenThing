#pragma once
#include "core/mqtt.h"
#include "modules/base/module.h"

#include <string>

namespace modules {
    class Humidity : public virtual Module {
        public:
            Humidity(std::string name, int pin, bool inverted = false, int zeroValue = 0, int maxValue = 4096);
            ~Humidity();

            virtual void   init() override;
            virtual void   loop() override;
            virtual void   userLoop() override;

            virtual double getHumidity();

        protected:
            int    pin;
            bool   inverted  = false; // Inverted logic for the sensor, if true, 0% humidity is represented by maxValue and 100% by zeroValue

            int    zeroValue = 0;    // Value representing 0% humidity
            int    maxValue  = 1024; // Value representing 100% humidity

            double humidity  = 0.0; // Current humidity value in percentage

#ifdef USE_MQTT
            core::mqtt::hass_data          hd        = { Module::getName() + "_humidity", "%", "humidity" };
            core::mqtt::mqtt_data<double> *mqtt_data = new core::mqtt::mqtt_data<double>(Module::getName() + "/humidity", 10000, hd);
#endif
    };
} // namespace modules