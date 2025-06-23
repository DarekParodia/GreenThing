#include "flow_meter.h"
#include <Arduino.h>

namespace mod{
    FlowMeter::FlowMeter(std::string name, int pin, int triggerPoint){
        this->name = name;
        this->pin = pin;
        this->triggerPoint = triggerPoint;
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
            }

            this->lastState = currentState;
        }
    }

    void FlowMeter::userLoop(){
        this->lastPulsesPerUserLoop = this->currentPulsesPerUserLoop;
        this->currentPulsesPerUserLoop = 0;
        Serial.println(this->lastPulsesPerUserLoop);
    }

    void FlowMeter::registerPulse(){
        this->currentPulsesPerUserLoop += 1;
    }
}