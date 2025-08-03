#pragma once

#include "core/mqtt.h"
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

            inline double getVolume24H() {
                return volumeLast24H;
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

            double        volumeLast24H           = 0.0;
            int           lastDay                 = 0;

            // Custom Measurments
            std::vector<double *> volumeMeasurments;

#ifdef USE_MQTT
            core::mqtt::hass_data          hd_flow   = { Module::getName() + "_flow", "L/min", "volume_flow_rate" };
            core::mqtt::hass_data          hd_vol    = { Module::getName() + "_volume", "L", "water" };
            core::mqtt::mqtt_data<double> *mqtt_flow = new core::mqtt::mqtt_data<double>(Module::getName() + "/flow", 1000, hd_flow);
            core::mqtt::mqtt_data<double> *mqtt_vol  = new core::mqtt::mqtt_data<double>(Module::getName() + "/volume", 1000, hd_vol);
#endif

            void registerPulse();
    };
} // namespace modules