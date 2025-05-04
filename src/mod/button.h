#pragma once

#include "mod/module.h"

namespace mod
{
    class Button : public Module
    {
    public:
        Button(int pin, bool inverted = false, bool bistable = false);
        ~Button();

        void init() override;
        void loop() override;
        bool isPressed() const;

    private:
        int pin;
        bool state;
        bool inverted;
        bool bistable;
        bool lastState = false; // Last state of the button
        bool debounce();
        unsigned long lastDebounceTime = 0; // Last time the button state was toggled
        unsigned long debounceDelay = 10; // Debounce time in milliseconds
    };
}