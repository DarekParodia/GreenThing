#pragma once
#include "module.h"
#include "core/api/datapoint.h"
#include <ArduinoJson.h>
#include <string>

namespace mod
{
    class Led : public Module, public core::api::DataPoint
    {
        public:
        Led(std::string name, int pin, bool inverted = false, long interval = -1);
        ~Led();

        void init() override;
        void loop() override;
        void setState(bool state);
        void toggle();
        void on();
        void off();
        void setToggleInterval(long interval);
        void addToJsonArray(JsonArray& array) override;
        void handleGetRequest(const std::string& requestBody);
        void handlePostRequest(const std::string& requestBody) override;

    private:
        int pin = -1; // Pin for the LED
        bool inverted = false; // Inverted logic
        bool state = false; // State of the LED
        long toggleInterval = -1; // Interval for toggling the LED (in milliseconds)
        unsigned long lastToggleTime = 0; // Last time the LED was toggled
    
    protected:
        core::api::DataPointType type = core::api::DataPointType::LED; // Set the type to LED
    };
}
