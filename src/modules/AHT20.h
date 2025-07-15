#pragma once
#include "modules/base/humidity.h"
#include "modules/base/module.h"
#include "modules/base/temperature.h"

#include <string>

namespace modules {
    class AHT20 : public modules::Temperature,
                  public modules::Humidity {
        public:
            AHT20(std::string name);
            AHT20(std::string name, unsigned long update_frequency);
            ~AHT20();

            void init();
            void loop();
            void userLoop();

        private:
            void trigger();
            void measure();
    };
} // namespace modules