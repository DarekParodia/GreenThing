#pragma once

#include "modules/base/module.h"

#include <string>

namespace modules {
    class Led : public Module {
        public:
            Led(std::string name, int pin, bool inverted = false, long interval = -1);
            ~Led();

            void init() override;
            void loop() override;
            void setState(bool state);
            void toggle();
            void on();
            void off();
            void setToggleInterval(long interval);

        private:
            int           pin            = -1;    // Pin for the LED
            bool          inverted       = false; // Inverted logic
            bool          state          = false; // State of the LED
            long          toggleInterval = -1;    // Interval for toggling the LED (in milliseconds)
            unsigned long lastToggleTime = 0;     // Last time the LED was toggled
    };
} // namespace modules
