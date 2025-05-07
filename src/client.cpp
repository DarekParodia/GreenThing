#include "client.h"
#include <Arduino.h>
#include "mod/button.h"
#include "mod/solenoid.h"
#include "mod/led.h"
#include "core/core.h"

mod::Button button(5, false, true);         // Button on pin 2, inverted logic
mod::Solenoid solenoid(16, 14, false, 250); // Solenoid on pin 16 and 14, not inverted, pulse time 100ms
mod::Led led(2, false, 1000);           // LED on pin 13, not inverted, toggle interval 1000ms
namespace client
{
    void setup()
    {
        core::addModule(&button);
        core::addModule(&solenoid);
        core::addModule(&led);
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

        if (solenoid.isOpen())
        {
            led.on();
        }
    }
}