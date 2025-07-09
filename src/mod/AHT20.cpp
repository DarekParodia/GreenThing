#include "AHT20.h"
#include <Arduino.h>

namespace mod{
    AHT20::AHT20(std::string name)
        : Module(), Temperature(), Humidity(name, -1, false, 0, 100) // -1 pin, not used
    {
        Module::name = name;
    }

    AHT20::~AHT20(){}

    void AHT20::init(){
        if(!this->aht.begin()){
            Serial.println("Failed to initialize AHT20!");
        }
    }

    void AHT20::loop(){
        aht.getEvent(&humidity_event, &temp_event);
        this->temperature = temp_event.temperature;
        this->humidity = humidity_event.relative_humidity;
    }

    void AHT20::userLoop(){
        // Optional: implement if needed
    }
}