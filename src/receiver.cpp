#ifdef RECEIVER
#include "receiver.h"
#include <Arduino.h>
#include "mod/button.h"
#include "core/core.h"

mod::Button button(5, false, true); // Button on pin 2, inverted logic

int pin = 16;

namespace receiver
{
    void setup()
    {
        pinMode(pin, OUTPUT);
        core::addModule(&button);
    }

    void loop()
    {
        if (button.isPressed())
        {
            digitalWrite(pin, HIGH);
        } else{
            digitalWrite(pin, LOW);
        }
    }
}
#endif