#pragma once
#include "module.h"
#include <string>

namespace mod {
    class FlowMeter : public Module {
        public:
            FlowMeter(std::string name, int pin, int triggerPoint = 512);
            ~FlowMeter();

            void init() override;
            void loop() override;
            void userLoop();

        private:
            std::string name;
            int pin;
            int triggerPoint = 512;
            int val = 0;
            bool lastState = false;
            unsigned long lastPulseTime = 0;

            void registerPulse();
    };
}