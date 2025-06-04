#include "solenoid.h"

#include <Arduino.h>
#include "core/core.h"
#include "core/api/api.h"

namespace mod
{
    Solenoid::Solenoid(std::string name, int pin, int pin2, bool inverted, int pulseTime) : core::api::DataPoint(name)
    {
        this->pin1 = pin;
        this->pin2 = pin2;
        if (pin2 != -1)
            this->bistable = true;
        this->inverted = inverted;
        this->pulseTime = pulseTime;
    }

    Solenoid::~Solenoid() {}

    void Solenoid::init()
    {
        pinMode(pin1, OUTPUT);
        if (bistable)
        {
            pinMode(pin2, OUTPUT);
        }
        core::api::addDataPoint(this); // Add this solenoid to the data points
    }

    void Solenoid::loop()
    {
    }

    void Solenoid::setState(bool state)
    {
        if (isTimeouted) return; // If the timeout is active, do not change the state
        if (bistable)
        {
            core::timeout_t *timeout = new core::timeout_t();
            timeout->delay = pulseTime;
            timeout->callback = [](void *custom_pointer)
            {
                auto *solenoid = static_cast<Solenoid *>(custom_pointer);
                solenoid->disableOutputs();
            };
            timeout->custom_pointer = this; // Pass the current object as custom pointer
            core::addTimeout(timeout);
            isTimeouted = true; // Set the timeout flag
            digitalWrite((state ^ inverted) ? pin1 : pin2, HIGH);
        }
        else
        {
            digitalWrite(pin1, state ^ inverted ? HIGH : LOW);
        }
        this->state = state;
    }
    void Solenoid::open()
    {
        if (state)
            return;
        this->setState(true);
    }
    void Solenoid::close()
    {
        if (!state)
            return;
        this->setState(false);
    }
    void Solenoid::toggle()
    {
        this->setState(!state);
    }
    void Solenoid::disableOutputs()
    {
        digitalWrite(pin1, LOW);
        if (bistable)
        {
            digitalWrite(pin2, LOW);
        }
        isTimeouted = false; // Reset the timeout flag
    }
    bool Solenoid::isOpen()
    {
        return state;
    }

    void Solenoid::addToJsonArray(JsonArray &array)
    {
        JsonObject solenoidObject = array.add<JsonObject>();
        solenoidObject["name"] = this->name.c_str();
        solenoidObject["type"] = (int)this->type;
        solenoidObject["state"] = state;
        solenoidObject["pin1"] = pin1;
        solenoidObject["pin2"] = pin2;
        solenoidObject["bistable"] = bistable;
        solenoidObject["inverted"] = inverted;
        solenoidObject["pulseTime"] = pulseTime;
    }

    void Solenoid::handleGetRequest(const std::string &requestBody)
    {
        // Handle GET request to return solenoid state
        Serial.println("Handling GET request for Solenoid");
        Serial.println(std::string("Request Body: " + requestBody).c_str());
    }

    void Solenoid::handlePostRequest(const std::string &requestBody)
    {
        // Parse the request body to get the new state
        Serial.println("Handling POST request for Solenoid");
        Serial.println(std::string("Request Body: " + requestBody).c_str());
    }
}