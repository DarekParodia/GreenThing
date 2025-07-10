#pragma once
#include "module.h"
#include "ultrasonic.h"

#include <string>
#include <RCWL_1X05.h>

namespace mod {
    class RCWL_1x05 : public Ultrasonic {
        public:
            RCWL_1x05(std::string name);
            ~RCWL_1x05();

            void init() override;
            void loop() override;
            void userLoop() override;

            double getDistance() override;

            inline void setTemperature(int16_t temperature){
                this->sensor->setTemperature(temperature);
            }

        private:
            double distance = 0.0; // Distance in cm
            std::string name;
            bool failed = false; // Flag to indicate if the sensor failed to initialize

            RCWL_1X05 *sensor = nullptr; // Initialize pointer to nullptr
    };
}