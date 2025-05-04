#include "solenoid.h"

#include <Arduino.h>
#include "core/core.h"

namespace mod
{
    Solenoid::Solenoid(int pin, int pin2, bool inverted, int pulseTime)
    {
        this->pin1 = pin1;
        this->pin2 = pin2;
        if (pin2 != -1)
            this->bistable = true;
        this->inverted = inverted;
        this->pulseTime = pulseTime;
    }

    Solenoid::~Solenoid() {}

    void Solenoid::init()
    {
        pinMode(pin1, OUTPUT);
        if (bistable)
        {
            pinMode(pin2, OUTPUT);
        }
    }

    void Solenoid::loop()
    {
        
    }

    void Solenoid::setState(bool state)
    {
        if (bistable)
        {
            core::timeout_t timeout;
            timeout.delay = pulseTime;
            timeout.callback = [](void *arg) {
                auto *solenoid = static_cast<Solenoid *>(arg);
                solenoid->disableOutputs();
            };
            core::addTimeout(&timeout);
            digitalWrite((state ^ inverted) ? pin1 : pin2, HIGH);
        }
        else
        {
            digitalWrite(pin1, state ^ inverted ? HIGH : LOW);
        }
        this->state = state;
    }
    void Solenoid::open()
    {
       this->setState(true);
    }
    void Solenoid::close()
    {
        this->setState(false);
    }
    void Solenoid::toggle()
    {
        this->setState(!state);
    }
    void Solenoid::disableOutputs()
    {
        digitalWrite(pin1, LOW);
        if (bistable)
        {
            digitalWrite(pin2, LOW);
        }
    }
    bool Solenoid::isOpen() 
    {
        return state;
    }
}