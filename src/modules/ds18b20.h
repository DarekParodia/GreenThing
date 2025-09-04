#pragma once
#include "modules/base/module.h"
#include "modules/base/temperature.h"

#include <DallasTemperature.h>
#include <OneWire.h>
#include <string>

namespace modules {
    class DS18B20 : public modules::Temperature {
        public:
            DS18B20(std::string name, int pin);
            DS18B20(std::string name, unsigned long update_frequency, int pin);
            ~DS18B20();

            void init();
            void loop();
            void userLoop();

        private:
            int                pin = -1;
            void               trigger();
            void               measure();
            bool               failed;
            OneWire           *oneWire;
            DallasTemperature *sensor;
    };
} // namespace modules