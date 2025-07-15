#pragma once

#include "modules/base/module.h"

#include <string>
#include <vector>
namespace modules {
    class FlowMeter : public virtual Module {
        public:
            FlowMeter(std::string name, int pin, int triggerPoint, double freqPerFlowFactor = 5.5);
            FlowMeter(std::string name, int pin, double freqPerFlowFactor = 5.5);
            ~FlowMeter();

            void          init() override;
            void          loop() override;
            void          userLoop() override;

            inline double getFlowRate() {
                return userLoopFlowRate;
            }

            inline double getVolume() {
                return userLoopVolume;
            }

            inline int startVolumeMeasurment(double *vol) {
                volumeMeasurments.push_back(vol);
                return volumeMeasurments.size() - 1;
            }

        private:
            int pin;

            // Loop Stuff
            int  triggerPoint                     = 512;
            int  val                              = 0;
            bool lastState                        = false;
            bool digital                          = false;

            // UserLoop
            unsigned int currentPulsesPerUserLoop = 0;
            unsigned int lastPulsesPerUserLoop    = 0;

            double       userLoopFlowRate         = 0.0; // Flow rate in liters per minute (L/min) for the user loop
            double       userLoopFrequency        = 0.0; // Frequency in hertz for the user loop
            double       userLoopVolume           = 0.0; // Volume in liters for the user loop

            // Flow Calculation
            double        freqPerFlowFactor       = 5.5; // hertz per liter per second
            double        freq                    = 0.0; // Frequency in hertz
            double        volume                  = 0.0; // Volume in liters (per last user loop)
            double        flowRate                = 0.0; // Flow rate in liters per minute (L/min)
            unsigned long lastPulseTime           = 0;   // Time of the last pulse in microseconds

            // Custom Measurments
            std::vector<double *> volumeMeasurments;

            void                  registerPulse();
    };
} // namespace modules