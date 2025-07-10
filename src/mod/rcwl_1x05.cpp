#include "rcwl_1x05.h"
#include <Arduino.h>
#include <Wire.h>

namespace mod {
    RCWL_1x05::RCWL_1x05(std::string name)
        : Ultrasonic(name, -1, -1)
    {
        this->name = name;
        this->sensor = new RCWL_1X05();
    }

    RCWL_1x05::~RCWL_1x05() {}
    
    void RCWL_1x05::init() {
        // Do not call Wire.begin() here! It is already called in main setup.
        bool result = this->sensor->begin();

        if(!result) {
            Serial.println("RCWL_1X05 sensor initialization failed!");
            this->failed = true; // Set the failed flag
            return; // Exit if initialization failed
        }

        this->sensor->setMode(RCWL_1X05::continuous); // Set mode to triggered
        this->sensor->setTemperature(20); // Set default temperature
        this->sensor->setTimeout(200); // Set timeout to 200 ms
        this->sensor->setFilter(false);
        this->sensor->trigger(); // Trigger the first measurement
    }

    void RCWL_1x05::loop() {
        // bool newData = sensor->update(); // calling update() repeatedly is crucial in continuous mode
        // if (newData) {
        //     unsigned int distance = sensor->read();
        //     if(distance != 0)
        //         this->distance = (double) distance / 10.0; // Convert mm to cm
        // }
    }

    void RCWL_1x05::userLoop() {

    }

    double RCWL_1x05::getDistance() {
        return this->distance; // Return the distance in cm
    }
}