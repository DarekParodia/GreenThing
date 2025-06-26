#pragma once
#include "module.h"
#include <string>

namespace mod {
    class FlowMeter : public Module {
        public:
            FlowMeter(std::string name, int pin, int triggerPoint = 512, double freqPerFlowFactor = 5.5);
            ~FlowMeter();

            void init() override;
            void loop() override;
            void userLoop() override;

        private:
            std::string name;
            int pin;

            // Loop Stuff
            int triggerPoint = 512;
            int val = 0;
            bool lastState = false;

            // UserLoop
            unsigned int currentPulsesPerUserLoop = 0;
            unsigned int lastPulsesPerUserLoop = 0;

            double userLoopFlowRate = 0.0; // Flow rate in liters per minute (L/min) for the user loop
            double userLoopFrequency = 0.0; // Frequency in hertz for the user loop
            double userLoopVolume = 0.0; // Volume in liters for the user loop

            // Flow Calculation
            double freqPerFlowFactor = 5.5;   // hertz per liter per second
            double freq = 0.0; // Frequency in hertz
            double volume = 0.0; // Volume in liters (per last user loop)
            double flowRate = 0.0; // Flow rate in liters per minute (L/min)
            unsigned long lastPulseTime = 0; // Time of the last pulse in microseconds

            void registerPulse();
    };
}