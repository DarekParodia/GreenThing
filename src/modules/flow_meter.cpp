#include "core/core.h"
#include "core/time.h"
#include "flow_meter.h"

#include <Arduino.h>

namespace modules {
    FlowMeter::FlowMeter(std::string name, int pin, int triggerPoint, double freqPerFlowFactor) :
        modules::Module(name) {
        this->pin               = pin;
        this->triggerPoint      = triggerPoint;
        this->freqPerFlowFactor = freqPerFlowFactor;
        this->digital           = false;
    }

    FlowMeter::FlowMeter(std::string name, int pin, double freqPerFlowFactor) :
        modules::Module(name) {
        this->pin               = pin;
        this->freqPerFlowFactor = freqPerFlowFactor;
        this->digital           = true;
    }

    FlowMeter::~FlowMeter() {
    }

    void FlowMeter::init() {
        if(!this->digital)
            pinMode(this->pin, INPUT);
        else
            pinMode(this->pin, INPUT_PULLUP);
    }

    void FlowMeter::loop() {
        if(!digital) {
            this->val         = analogRead(this->pin);
            bool currentState = false;

            if(this->val >= this->triggerPoint) currentState = true;

            if(this->lastState != currentState) {
                if(!currentState && this->lastState) {
                    this->registerPulse();
                    // Serial.print("pulse ");
                    // Serial.println(millis());
                }

                this->lastState = currentState;
            }
        } else {
            bool currentState = digitalRead(this->pin) == LOW ? 1 : 0;
            // Serial.print("State: ");
            // Serial.println(digitalRead(this->pin) );

            if(this->lastState != currentState) {
                if(!currentState && this->lastState) {
                    this->registerPulse();
                    // Serial.print("pulse ");
                    // Serial.println(millis());
                }

                this->lastState = currentState;
            }
        }
    }

    void FlowMeter::userLoop() {
        this->lastPulsesPerUserLoop    = this->currentPulsesPerUserLoop;
        this->currentPulsesPerUserLoop = 0;

        this->userLoopFrequency        = this->lastPulsesPerUserLoop * (core::userDeltaTime / 1000000.0); // Frequency in hertz
        this->userLoopFlowRate         = this->userLoopFrequency / this->freqPerFlowFactor;               // Flow rate in liters per minute
        this->userLoopVolume           = this->volume;
        this->volume                   = 0.0; // Reset volume for the next user loop

        if(core::time->getDay() != lastDay) {
            lastDay             = core::time->getDay();
            this->volumeLast24H = 0.0;
        }

        this->volumeLast24H += this->userLoopVolume;

// update mqtt
#ifdef USE_MQTT
        this->mqtt_vol->update(volumeLast24H, true);
        this->mqtt_flow->update(userLoopFlowRate, true);
#endif

        // Serial.printf(
        // "Pulses: %d | Frequency: %f Hz | Flow Rate: %f L/min | Volume: %.2f L | Time: %.2f ms | Loop Time: %.2f ms\n",
        // this->lastPulsesPerUserLoop,
        // this->userLoopFrequency,
        // this->userLoopFlowRate,
        // this->userLoopVolume,
        // core::userDeltaTime / 1000.0,
        // core::deltaTime / 1000.0);
    }

    void FlowMeter::registerPulse() {
        this->currentPulsesPerUserLoop += 1;
        unsigned long currentTime       = micros();

        if(this->lastPulseTime == 0 || currentTime < this->lastPulseTime) {
            // Reset if the last pulse time is not set or if the current time is less than the last pulse time
            this->lastPulseTime = currentTime;
            return;
        }

        // Calculate frequency and volume based on the time difference since the last pulse
        unsigned long timeDiff  = currentTime - this->lastPulseTime;
        this->freq              = 1000000.0 / timeDiff;                   // Frequency in hertz
        this->flowRate          = (this->freq / this->freqPerFlowFactor); // Flow rate in liters per minute

        double tempVol          = (this->flowRate * (timeDiff / 60000000.0)); // Volume in liters
        this->volume           += tempVol;

        for(int i = 0; i < this->volumeMeasurments.size(); i++)
            *volumeMeasurments[i] += tempVol;

        this->lastPulseTime = currentTime;
    }
} // namespace modules