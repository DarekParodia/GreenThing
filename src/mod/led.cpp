#include "led.h"
#include <Arduino.h>
#include "core/core.h"
#include "core/api/api.h"
namespace mod
{
    Led::Led(std::string name, int pin, bool inverted, long interval) : core::api::DataPoint(name)
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
            if (millis() - lastToggleTime >= toggleInterval)
            {
                lastToggleTime = millis(); // Update the last toggle time
                toggle();               // Toggle the LED state
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
        // ledObject["name"] = this->name.c_str();
        ledObject["type"] = (int)this->type;
        ledObject["state"] = state;
        ledObject["pin"] = pin;
        ledObject["inverted"] = inverted;
        ledObject["toggleInterval"] = toggleInterval;
    }

    void Led::handleGetRequest(const std::string &requestBody)
    {
        Serial.println("Handling GET request for LED");
        Serial.println(std::string("Request Body: " + requestBody).c_str());
    }

    void Led::handlePostRequest(const std::string &requestBody)
    {
        Serial.println("Handling POST request for LED");
        Serial.println(std::string("Request Body: " + requestBody).c_str());
    }

}
