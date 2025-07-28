#pragma once
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
    };
} // namespace modules