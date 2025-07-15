#include "ultrasonic.h"

#include <Arduino.h>

namespace modules {
    Ultrasonic::Ultrasonic(std::string name, int triggerPin, int echoPin) :
        modules::Module(name) {
        this->triggerPin = triggerPin;
        this->echoPin    = echoPin;
    }

    Ultrasonic::~Ultrasonic() {}

    void Ultrasonic::init() {
        pinMode(triggerPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }

    void Ultrasonic::loop() {
    }

    void Ultrasonic::userLoop() {
        // User-defined loop logic can be added here
    }

    double Ultrasonic::getDistance() {
        // Trigger the ultrasonic sensor
        digitalWrite(triggerPin, LOW);
        delayMicroseconds(2);
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(triggerPin, LOW);

        // Read the echo time
        long duration   = pulseIn(echoPin, HIGH, 30000); // Timeout after 30ms

        // Calculate distance in cm (speed of sound is 343 m/s)
        double distance = (duration * 0.0343) / 2; // Divide by 2 for round trip

        return distance;
    }
} // namespace modules