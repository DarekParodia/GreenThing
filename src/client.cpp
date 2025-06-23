#include "client.h"
#include <Arduino.h>
#include "mod/button.h"
#include "mod/solenoid.h"
#include "mod/led.h"
#include "mod/flow_meter.h"
#include "core/core.h"

mod::Button button("button1", 5, true, true);         // Button on pin 2, inverted logic
mod::Solenoid solenoid("solenoid1", 16, 14, false, 250); // Solenoid on pin 16 and 14, not inverted, pulse time 100ms
mod::Led led("led1", 2, false, 1000);           // LED on pin 13, not inverted, toggle interval 1000ms
mod::FlowMeter flow_meter("flow_meter1", 2);
namespace client
{
    void setup()
    {
        core::addModule(&button);
        core::addModule(&solenoid);
        core::addModule(&led);
        core::addModule(&flow_meter);
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