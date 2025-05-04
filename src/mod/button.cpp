#include "button.h"
#include <Arduino.h>

namespace mod
{
    Button::Button(int pin, bool inverted, bool bistable)
    {
        this->pin = pin;
        this->inverted = inverted;
        this->state = inverted ? HIGH : LOW; // Initialize state based on inverted logic
        this->bistable = bistable;
    }
    Button::~Button() {}

    void Button::init()
    {
        pinMode(pin, INPUT_PULLUP);
    }

    void Button::loop()
    {
        int readValue = digitalRead(pin) ^ inverted; // Read the button state and apply inversion if needed

        if (debounce())
        {
            if (bistable)
            {
                if (readValue != lastState && readValue == inverted)
                {
                    state = !state; // Toggle state
                    
                } 
                lastState = readValue; // Update last state
            } else{
                state = readValue;
            }
        }
    }

    bool Button::isPressed() const
    {
        return state;
    }

    bool Button::debounce()
    {
        unsigned long currentTime = millis();
        if ((currentTime - lastDebounceTime) > debounceDelay)
        {
            lastDebounceTime = currentTime;
            return true;
        }
        return false;
    }
}