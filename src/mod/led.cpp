#include "led.h"
#include <Arduino.h>
#include "core/core.h"
#include "core/api/api.h"
namespace mod
{
    Led::Led(std::string, int pin, bool inverted, long interval) : core::api::DataPoint(name)
    {
        this->pin = pin;
        this->inverted = inverted;
        this->toggleInterval = interval;
    }
    Led::~Led() {}

    void Led::init()
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, inverted ? HIGH : LOW); // Set initial state based on inverted logic
        core::api::addDataPoint(this);            // Add this LED to the data points
    }

    void Led::loop()
    {
        if (toggleInterval > 0)
        {
            if (core::loopCount % (unsigned long)(core::loopsPerMillisecond * (double)toggleInterval) == 0)
            {
                toggle(); // Toggle the LED state at the specified interval
            }
        }
    }
    void Led::setState(bool state)
    {
        this->state = state;
        digitalWrite(pin, inverted ? !state : state); // Set the LED state based on inverted logic
    }
    void Led::toggle()
    {
        state = !state;
        digitalWrite(pin, inverted ? !state : state); // Toggle the LED state based on inverted logic
    }
    void Led::on()
    {
        setState(true);
    }
    void Led::off()
    {
        setState(false);
    }
    void Led::setToggleInterval(long interval)
    {
        this->toggleInterval = interval;
    }

    void Led::addToJsonArray(JsonArray &array)
    {
        JsonObject ledObject = array.add<JsonObject>();
        ledObject["name"] = this->name.c_str();
        ledObject["type"] = (int)this->type;
        ledObject["state"] = state;
        ledObject["pin"] = pin;
        ledObject["inverted"] = inverted;
        ledObject["toggleInterval"] = toggleInterval;
    }
}
