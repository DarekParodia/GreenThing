#include "led.h"
#include <Arduino.h>
#include "core/core.h"

namespace mod
{
    Led::Led(int pin, bool inverted, long interval)
    {
        this->pin = pin;
        this->inverted = inverted;
        this->toggleInterval = interval;
    }
    Led::~Led() {}

    void Led::init()
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, inverted ? HIGH : LOW); // Set initial state based on inverted logic
    }

    void Led::loop()
    {
        if (toggleInterval > 0)
        {
            if (core::loopCount % (unsigned long)(core::loopsPerMillisecond * (double)toggleInterval) == 0)
            {
                toggle(); // Toggle the LED state at the specified interval
            }
        } 
    }
    void Led::setState(bool state)
    {
        this->state = state;
        digitalWrite(pin, inverted ? !state : state); // Set the LED state based on inverted logic
    }
    void Led::toggle()
    {
        state = !state;
        digitalWrite(pin, inverted ? !state : state); // Toggle the LED state based on inverted logic
    }
    void Led::on()
    {
        setState(true);
    }
    void Led::off()
    {
        setState(false);
    }
    void Led::setToggleInterval(long interval)
    {
        this->toggleInterval = interval;
    }
}
