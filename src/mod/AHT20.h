#pragma once
#include "module.h"
#include "temperature.h"
#include "humidity.h"

#include <string>

namespace mod {
class AHT20 : public virtual Module, public Temperature, public Humidity {
    public:
        AHT20(std::string name);
        ~AHT20();

        void init();
        void loop();  
        void userLoop();
    private:
        void trigger();
        void measure();
};
} // namespace mod