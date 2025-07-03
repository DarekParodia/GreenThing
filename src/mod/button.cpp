#include "button.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <string>
#include "core/api/api.h"

namespace mod
{
    Button::Button(std::string name, int pin, bool inverted, bool bistable) : core::api::DataPoint(name)
    {
        this->pin = pin;
        this->inverted = inverted;
        this->state = inverted ? HIGH : LOW; // Initialize state based on inverted logic
        this->bistable = bistable;

        core::api::addDataPoint(this); // Add this button to the data points
    }
    Button::~Button() {}

    void Button::init()
    {
        pinMode(pin, INPUT_PULLUP);
    }

    void Button::loop()
    {
        int readValue = digitalRead(pin) ^ inverted; // Read the button state and apply inversion if needed

        if (debounce())
        {
            if (bistable)
            {
                if (readValue != lastState && readValue == inverted)
                {
                    state = !state; // Toggle state
                }
                lastState = readValue; // Update last state
            }
            else
            {
                state = readValue;
            }
        }
    }

    bool Button::isPressed() const
    {
        return state;
    }

    bool Button::debounce()
    {
        unsigned long currentTime = millis();
        if ((currentTime - lastDebounceTime) > debounceDelay)
        {
            lastDebounceTime = currentTime;
            return true;
        }
        return false;
    }

    void Button::addToJsonArray(JsonArray &array)
    {
        // Create a JSON object to represent the button state
        JsonObject obj = array.add<JsonObject>();
        // obj["name"] = this->name.c_str();
        obj["type"] = (int)this->type;
        obj["state"] = state;
        obj["inverted"] = inverted;
        obj["bistable"] = bistable;
        obj["pin"] = pin;
    }
    void Button::handleGetRequest(const std::string &requestBody)
    {
        // Handle GET request to return button state
        // This is a placeholder for future implementation
        Serial.println("GET request received for Button");
        Serial.println(std::string("Request Body: " + requestBody).c_str());
    }

    void Button::handlePostRequest(const std::string &requestBody)
    {
        // Handle POST request to change button state or settings
        // This is a placeholder for future implementation
        Serial.println("POST request received for Button");
        Serial.println(std::string("Request Body: " + requestBody).c_str());
    }

}