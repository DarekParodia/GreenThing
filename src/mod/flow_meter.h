#pragma once
#include "module.h"
#include <string>

namespace mod {
    class FlowMeter : public Module {
        public:
            FlowMeter(std::string name, int pin);
            ~FlowMeter();

            void init() override;
            void loop() override;
            void userLoop();

        private:
            std::string name;
            int pin;
            bool lastState;
            unsigned int currentPulseCount = 0;
            unsigned int lastUserLoopPulseCount = 0;
    };
}