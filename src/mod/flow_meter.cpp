#include "flow_meter.h"
#include <Arduino.h>

namespace mod{
    FlowMeter::FlowMeter(std::string name, int pin){
        this->name = name;
        this->pin = pin;
    }

    FlowMeter::~FlowMeter(){

    }

    void FlowMeter::init(){
        pinMode(this->pin, INPUT);
    }

    void FlowMeter::loop(){

    }

    void FlowMeter::userLoop(){

    }
}