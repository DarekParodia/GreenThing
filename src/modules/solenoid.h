#pragma once

#include "modules/base/module.h"

#include <string>

namespace modules {
    class Solenoid : public Module {
        public:
            Solenoid(std::string name, int pin, int pin2 = -1, bool inverted = false, int pulseTime = 100);
            ~Solenoid();

            void init() override;
            void loop() override;
            void userLoop() override;

            void setState(bool state);
            void open();
            void close();
            void toggle();
            void disableOutputs();
            bool isOpen();

        private:
            int  pin1        = -1;
            int  pin2        = -1;
            bool bistable    = false;
            bool isTimeouted = false; // Flag to check if the timeout is active
            bool state       = false; // State of the solenoid
            bool inverted    = false; // Inverted logic
            int  pulseTime   = 100;   // Pulse time in millis
    };
} // namespace modules