#pragma once
#include "modules/base/module.h"

#include <string>

namespace modules {
    class Ultrasonic : public Module {
        public:
            Ultrasonic(std::string name, int triggerPin, int echoPin);
            ~Ultrasonic();

            void           init() override;
            void           loop() override;
            void           userLoop() override;

            virtual double getDistance();

        private:
            int    triggerPin;
            int    echoPin;

            double distance = 0.0; // Distance in cm
    };
} // namespace modules