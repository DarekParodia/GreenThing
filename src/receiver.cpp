#include "receiver.h"
#include <Arduino.h>

namespace receiver {
    void setup() {
        Serial.println("Receiver setup");
        // Initialize receiver hardware here
    }

    void loop() {
        // Check for incoming data and process it
        if (Serial.available()) {
            String data = Serial.readStringUntil('\n');
            Serial.print("Received: ");
            Serial.println(data);
        }
    }
}