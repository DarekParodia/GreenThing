#include "core/core.h"
#include "led.h"

#include <Arduino.h>
namespace modules {
    Led::Led(std::string name, int pin, bool inverted, long interval) :
        modules::Module(name) {
        this->pin            = pin;
        this->inverted       = inverted;
        this->toggleInterval = interval;
    }
    Led::~Led() {}

    void Led::init() {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, inverted ? HIGH : LOW); // Set initial state based on inverted logic
    }

    void Led::loop() {
        if(toggleInterval > 0) {
            if(millis() - lastToggleTime >= toggleInterval) {
                lastToggleTime = millis(); // Update the last toggle time
                toggle();                  // Toggle the LED state
            }
        }
    }
    void Led::setState(bool state) {
        this->state = state;
        digitalWrite(pin, inverted ? !state : state); // Set the LED state based on inverted logic
    }
    void Led::toggle() {
        state = !state;
        digitalWrite(pin, inverted ? !state : state); // Toggle the LED state based on inverted logic
    }
    void Led::on() {
        setState(true);
    }
    void Led::off() {
        setState(false);
    }
    void Led::setToggleInterval(long interval) {
        this->toggleInterval = interval;
    }

} // namespace modules
