#include "button.h"
#include <Arduino.h>

namespace mod
{
    Button::Button(int pin, bool inverted)
    {
        this->pin = pin;
        this->inverted = inverted;
        this->state = inverted ? HIGH : LOW; // Initialize state based on inverted logic
    }
    Button::~Button() {}

    void Button::init()
    {
        pinMode(pin, INPUT_PULLUP);
    }

    void Button::loop()
    {
        // Serial.print("Button loop state: ");
        this->state = (digitalRead(this->pin) == LOW) ^ this->inverted;
        // Serial.println(this->state ? "Pressed" : "Not Pressed");
    }

    bool Button::isPressed() const
    {
        return state;
    }
}