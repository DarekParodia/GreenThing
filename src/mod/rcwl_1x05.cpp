#include "rcwl_1x05.h"
#include <Arduino.h>
#include <Wire.h>

namespace mod {
    RCWL_1x05::RCWL_1x05(std::string name, int sda, int scl)
        : Ultrasonic(name, -1, -1), Module()
    {
        this->name = name;
        this->sda = sda; // SDA pin
        this->scl = scl; // SCL pin
        this->sensor = new RCWL_1X05();
    }

    RCWL_1x05::~RCWL_1x05() {}
    
    void RCWL_1x05::init() {
        Wire.begin(sda, scl); // Initialize I2C with specified SDA and SCL pins

        bool result = this->sensor->begin();

        if(!result) {
            Serial.println("RCWL_1X05 sensor initialization failed!");
            this->failed = true; // Set the failed flag
            return; // Exit if initialization failed
        }

        this->sensor->setMode(RCWL_1X05::triggered); // Set mode to oneShot
        this->sensor->setTemperature(20); // Set default temperature
        this->sensor->setTimeout(200); // Set timeout to 1000 ms
        this->sensor->setFilter(false);
        this->sensor->trigger(); // Trigger the first measurement
    }

    void RCWL_1x05::loop() {

    }

    void RCWL_1x05::userLoop() {
        if(this->failed) {
            // Serial.println("RCWL_1X05 sensor initialization failed, skipping measurement.");
            return; // Skip measurement if initialization failed
        }
        unsigned int distance = sensor->read();
        this->distance = (double) distance / 10.0; // Convert mm to cm

        Serial.println(("Distance: " + std::to_string(this->getDistance()) + " cm").c_str());

        this->sensor->trigger(); // Trigger a new measurement
    }

    double RCWL_1x05::getDistance() {
        return this->distance; // Return the distance in cm
    }
}