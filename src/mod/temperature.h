#pragma once
#include "module.h"
#include <string>

namespace mod {
class Temperature : public virtual Module {
    public:
        Temperature() = default;
        ~Temperature() = default;

        virtual void init() = 0;
        virtual void loop() = 0;  
        virtual void userLoop() {
                // To be overriden if needed
        }

        inline double getTemperature() { return temperature; }

    protected:
        double temperature = 0.0; // Current temperature value in celsius
};
} // namespace mod