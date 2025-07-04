#include "flow_meter.h"
#include "core/core.h"
#include <Arduino.h>

namespace mod{
    FlowMeter::FlowMeter(std::string name, int pin, int triggerPoint, double freqPerFlowFactor){
        this->name = name;
        this->pin = pin;
        this->triggerPoint = triggerPoint;
        this->freqPerFlowFactor = freqPerFlowFactor;
    }

    FlowMeter::~FlowMeter(){

    }

    void FlowMeter::init(){
        pinMode(this->pin, INPUT);
    }

    void FlowMeter::loop(){ 
        this->val = analogRead(this->pin);
        bool currentState = false;

        if (this->val >= this->triggerPoint) currentState = true;

        if (this->lastState != currentState){
            if(!currentState && this->lastState){  
                this->registerPulse();
                // Serial.print("pulse ");
                // Serial.println(millis());
            }

            this->lastState = currentState;
        }
    }

    void FlowMeter::userLoop(){
        this->lastPulsesPerUserLoop = this->currentPulsesPerUserLoop;
        this->currentPulsesPerUserLoop = 0;

        this->userLoopFrequency = this->lastPulsesPerUserLoop * (core::userDeltaTime / 1000000.0); // Frequency in hertz
        this->userLoopFlowRate = this->userLoopFrequency / this->freqPerFlowFactor; // Flow rate in liters per minute
        this->userLoopVolume = this->volume;
        this->volume = 0.0; // Reset volume for the next user loop

        // Serial.print("Pulses: ");
        // Serial.print(this->lastPulsesPerUserLoop);
        // Serial.print(" | Frequency: ");
        // Serial.print(this->userLoopFrequency);
        // Serial.print(" Hz | Flow Rate: ");
        // Serial.print(this->userLoopFlowRate);
        // Serial.print(" L/min | Volume: ");
        // Serial.print(this->userLoopVolume);
        // Serial.print(" L | Time: ");
        // Serial.print(core::userDeltaTime / 1000.0);
        // Serial.print(" ms | Loop Time: ");
        // Serial.print(core::deltaTime / 1000.0);
        // Serial.println(" ms");
    }

    void FlowMeter::registerPulse(){
        this->currentPulsesPerUserLoop += 1;
        unsigned long currentTime = micros();

        if (this->lastPulseTime == 0 || currentTime < this->lastPulseTime) {
            // Reset if the last pulse time is not set or if the current time is less than the last pulse time
            this->lastPulseTime = currentTime;
            return;
        }

        // Calculate frequency and volume based on the time difference since the last pulse
        unsigned long timeDiff = currentTime - this->lastPulseTime;
        this->freq = 1000000.0 / timeDiff; // Frequency in hertz
        this->flowRate = (this->freq / this->freqPerFlowFactor); // Flow rate in liters per minute

        double tempVol = (this->flowRate * (timeDiff / 60000000.0)); // Volume in liters 
        this->volume += tempVol;

        for(int i = 0; i < this->volumeMeasurments.size(); i++){
            *volumeMeasurments[i] += tempVol;
        }

        this->lastPulseTime = currentTime;
    }
}