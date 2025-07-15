#include "humidity.h"

#include <Arduino.h>

namespace modules {
    Humidity::Humidity(std::string name, int pin, bool inverted, int zeroValue, int maxValue) :
        modules::Module(name) {
        this->pin       = pin;
        this->inverted  = inverted; // Set the inverted logic for the sensor
        this->zeroValue = zeroValue;
        this->maxValue  = maxValue;
        this->humidity  = 0.0; // Initialize humidity to 0.0%
    }

    Humidity::~Humidity() {}

    void Humidity::init() {
        pinMode(pin, INPUT);
    }

    void Humidity::loop() {
        int rawValue = analogRead(pin);

        //    Serial.print("Raw Humidity Value: " + String(rawValue));

        if(inverted) {
            // Inverted logic: 0% humidity is represented by maxValue and 100% by zeroValue
            rawValue = maxValue - rawValue; // Invert the reading
        }

        // Calculate humidity percentage
        if(rawValue < zeroValue)
            rawValue = zeroValue; // Ensure we don't go below 0%
        else if(rawValue > maxValue)
            rawValue = maxValue; // Ensure we don't exceed 100%
        humidity = ((double) (rawValue - zeroValue) / (maxValue - zeroValue)) * 100.0;

        // Serial.print(" | Humidity: ");
        // Serial.print(humidity);
        // Serial.println("%");
    }

    void Humidity::userLoop() {
    }

    double Humidity::getHumidity() {
        return this->humidity;
    }
} // namespace modules