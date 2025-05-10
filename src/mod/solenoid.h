#pragma once

#include "mod/module.h"
#include "core/api/datapoint.h"
#include <ArduinoJson.h>    
#include <string>

namespace mod
{
    class Solenoid : public Module, public core::api::DataPoint
    {
    public:
        Solenoid(std::string name, int pin, int pin2 = -1, bool inverted = false, int pulseTime = 100);
        ~Solenoid();

        void init() override;
        void loop() override;
        void setState(bool state);
        void open();
        void close();
        void toggle();
        void disableOutputs();
        bool isOpen();
        void addToJsonArray(JsonArray &array) override;

    private:
        int pin1 = -1;
        int pin2 = -1;
        bool bistable = false;
        bool isTimeouted = false; // Flag to check if the timeout is active
        bool state = false; // State of the solenoid
        bool inverted = false; // Inverted logic
        int pulseTime = 100; // Pulse time in millis

    protected:
        core::api::DataPointType type = core::api::DataPointType::SOLENOID; // Set the type to SOLENOID
    };
}