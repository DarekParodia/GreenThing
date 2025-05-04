#ifdef RECEIVER
#include "receiver.h"
#include <Arduino.h>
#include "mod/button.h"
#include "mod/solenoid.h"
#include "core/core.h"

mod::Button button(5, false, true); // Button on pin 2, inverted logic
mod::Solenoid solenoid(16, 14, false, 1000); // Solenoid on pin 2 and 3, not inverted, pulse time 100ms

bool lastState = LOW;

namespace receiver
{
    void setup()
    {
        core::addModule(&button);
        core::addModule(&solenoid);
    }

    void loop()
    {
        if (button.isPressed())
        {
            if (button.isPressed() != lastState)
            {
                lastState = button.isPressed();
                solenoid.toggle();
            }
        }
    }
}
#endif