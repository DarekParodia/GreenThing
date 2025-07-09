#pragma once
#include "module.h"
#include "temperature.h"
#include "humidity.h"

#include <string>
#include <Adafruit_AHTX0.h>

namespace mod {
class AHT20 : public virtual Module, public Temperature, public Humidity {
    public:
        AHT20(std::string name);
        ~AHT20();

        void init();
        void loop();  
        void userLoop();

    private:
        Adafruit_AHTX0 aht;
        sensors_event_t humidity_event, temp_event;
};
} // namespace mod