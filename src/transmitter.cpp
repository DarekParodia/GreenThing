#ifdef TRANSMITTER
#include "transmitter.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial HCSerial(4, 5); // RX, TX

namespace transmitter
{
    void setup()
    {
        Serial.println("Transmitter setup");
        HCSerial.begin(9600);
    }

    void loop()
    {
        HCSerial.println("Hello from HC-12!"); // Send a message every second
        delay(1000);
    }
}
#endif