#pragma once
#include "module.h"
#include <string>

namespace mod {
    class Ultrasonic : public Module {
        public:
            Ultrasonic(std::string name, int triggerPin, int echoPin);
            ~Ultrasonic();

            void init() override;
            void loop() override;
            void userLoop() override;

        private:
            std::string name;
            int triggerPin;
            int echoPin;

            double distance = 0.0; // Distance in cm
    };
}