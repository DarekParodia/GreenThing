#include "client.h"
#include <Arduino.h>
#include <Wire.h>
#include "mod/button.h"
#include "mod/solenoid.h"
#include "mod/led.h"
#include "mod/flow_meter.h"
#include "mod/humidity.h"
#include "mod/ultrasonic.h"
#include "mod/rcwl_1x05.h"
#include "core/core.h"
#include "core/wifi/wifi.h"

#include <Wire.h>
#include "I2C_LCD.h"

I2C_LCD lcd(39);

mod::Button button("button1", 5, true, true);            // Button on pin 2, inverted logic
mod::Solenoid solenoid("solenoid1", 16, 14, false, 250); // Solenoid on pin 16 and 14, not inverted, pulse time 100ms
mod::Led led("led1", 2, false, 1000);                    // LED on pin 13, not inverted, toggle interval 1000ms
// mod::FlowMeter flow_meter("flow_meter1", 2);             // Flow meter on pin 2, default trigger point 512, default frequency per flow factor 5.5
// mod::Humidity humidity("humidity1", 4, true);            // Humidity sensor on pin 4
// mod::Ultrasonic ultrasonic("ultrasonic1", 12, 13); // Ultrasonic sensor on pin 12 (trigger) and pin 13 (echo)
// mod::Ultrasonic *ultrasonic = new mod::RCWL_1x05("rcwl1x05", 21, 22); // RCWL-1X05 ultrasonic sensor on SDA pin 21 and SCL pin 22

namespace client
{
    void setup()
    {
        Wire.begin(21, 22);

        core::addModule(&button);
        core::addModule(&solenoid);
        core::addModule(&led);
        // core::addModule(&flow_meter);
        // core::addModule(&humidity);
        // core::addModule(ultrasonic);

        lcd.begin(20, 4);
        lcd.clear();
        lcd.backlight();
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

        // Display IP
        lcd.setCursor(0, 0);
        if (core::wifi::isConnected())
            lcd.print(core::wifi::getStringIP().c_str());
        else
            lcd.print("Connecting...");
        
        
    }
}