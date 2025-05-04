#include "solenoid.h"

#include <Arduino.h>

namespace mod
{
    Solenoid(int pin, int pin2 = -1, bool inverted = false, int pulseTime = 100)
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
        if (state ^ inverted)
        {
            open();
        }
        else
        {
            close();
        }
    }
    void Solenoid::open()
    {
        if (bistable)
        {
      
        }
        else
        {
            digitalWrite(pin1, HIGH);
        }
        isOpen = true;
    }
    void Solenoid::close()
    {
        if (bistable)
        {
        
        }
        else
        {
            digitalWrite(pin1, LOW);
        }
        isOpen = false;
    }
    void Solenoid::toggle()
    {
        if (isOpen)
        {
            close();
        }
        else
        {
            open();
        }
    }
    bool Solenoid::isOpen() const
    {
        return isOpen;
    }
}