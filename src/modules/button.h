#pragma once

#include "modules/base/module.h"

namespace modules {
    class Button : public virtual Module {
        public:
            Button(std::string name, int pin, bool inverted = false, bool bistable = false);
            ~Button();

            void init() override;
            void loop() override;
            void userLoop() override;

            bool isPressed() const;

        private:
            int           pin;
            bool          state;
            bool          inverted;
            bool          bistable;
            bool          lastState = false; // Last state of the button
            bool          debounce();
            unsigned long lastDebounceTime = 0;  // Last time the button state was toggled
            unsigned long debounceDelay    = 10; // Debounce time in milliseconds
    };
} // namespace modules