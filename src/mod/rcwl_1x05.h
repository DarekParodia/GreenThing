#pragma once
#include "module.h"
#include "ultrasonic.h"

#include <string>
#include <RCWL_1X05.h>

namespace mod {
    class RCWL_1x05 : public Ultrasonic, public Module {
        public:
            RCWL_1x05(std::string name, int sda = -1, int scl = -1);
            ~RCWL_1x05();

            void init() override;
            void loop() override;
            void userLoop() override;

            double getDistance() override;

        private:
            double distance = 0.0; // Distance in cm
            std::string name;
            int sda; // SDA pin
            int scl; // SCL pin
            bool failed = false; // Flag to indicate if the sensor failed to initialize

            RCWL_1X05 *sensor = nullptr; // Initialize pointer to nullptr
    };
}