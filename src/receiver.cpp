#ifdef RECEIVER
#include "receiver.h"
#include <Arduino.h>
#include "mod/button.h"
#include "mod/solenoid.h"
#include "core/core.h"

mod::Button button(5, false, true);          // Button on pin 2, inverted logic
mod::Solenoid solenoid(16, 14, false, 100); // Solenoid on pin 16 and 14, not inverted, pulse time 100ms

int ledPin = 2; // Pin for the LED
int ledState = LOW; // Variable to store the LED state
namespace receiver
{
    void setup()
    {
        core::addModule(&button);
        core::addModule(&solenoid); 
        pinMode(ledPin, OUTPUT); // Set the LED pin as output
    }

    void loop()
    {
        if (button.isPressed())
        {
            solenoid.open();
        }
        else
        {
            solenoid.close();
        }

        if (core::loopCount % (1000000 / core::loopTime) == 0)
        {
            ledState = !ledState; // Toggle the LED state every second
            digitalWrite(ledPin, ledState); // Set the LED pin to the current state
        }
    }
}
#endif