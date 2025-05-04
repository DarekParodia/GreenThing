#ifdef RECEIVER
#include "receiver.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "hc12.h"
#include "mod/button.h"

SoftwareSerial HCSerial(4, 5); // RX, TX
HC12::HC12 hc12;
mod::Button button(5, false); // Button on pin 2, inverted logic

int pin = 16;

namespace receiver
{
    void setup()
    {
        Serial.println("Receiver setup");
        HCSerial.begin(9600);
        delay(1000);
        hc12.begin(HCSerial, HC12_SET_PIN); 
        Serial.println("HC-12 initialized");
        Serial.println("Current parameters:");
        Serial.println(hc12.getParameters());

        pinMode(pin, OUTPUT);
        button.init();
    }

    void loop()
    {
        Serial.println("Current parameters:");
        // Serial.println(hc12.getParameters());
        // delay(1000);

        button.loop();
        if (button.isPressed())
        {
            Serial.println("Button pressed");
            digitalWrite(pin, HIGH);
        } else{
            digitalWrite(pin, LOW);
        }
    }
}
#endif