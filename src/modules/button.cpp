#include "button.h"

#include <Arduino.h>
#include <string>

namespace modules {
    Button::Button(std::string name, int pin, bool inverted, bool bistable) :
        modules::Module(name) {
        this->pin      = pin;
        this->inverted = inverted;
        this->state    = inverted ? HIGH : LOW; // Initialize state based on inverted logic
        this->bistable = bistable;
    }
    Button::~Button() {}

    void Button::init() {
        pinMode(pin, INPUT_PULLUP);
    }

    void Button::loop() {
        int readValue = digitalRead(pin) ^
                        inverted; // Read the button state and apply inversion if needed

        if(debounce()) {
            if(bistable) {
                if(readValue != lastState && readValue == inverted)
                    state = !state;    // Toggle state
                lastState = readValue; // Update last state
            } else {
                state = readValue;
            }
        }
    }

    void Button::userLoop() {

    };

    bool Button::isPressed() const {
        return state;
    }

    bool Button::debounce() {
        unsigned long currentTime = millis();
        if((currentTime - lastDebounceTime) > debounceDelay) {
            lastDebounceTime = currentTime;
            return true;
        }
        return false;
    }

} // namespace modules