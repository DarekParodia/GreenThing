#pragma once

#include "core/mqtt.h"
#include "modules/base/module.h"

namespace modules {
    class Button : public virtual Module {
        public:
            Button(std::string name, int pin, bool inverted = false, bool bistable = false);
            ~Button();

            void init() override;
            void loop() override;
            void userLoop() override;

            void setState(bool state);

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
            bool          mqtt_state       = false;
#ifdef USE_MQTT
            core::mqtt::EntityMeta mqtt_meta   = { Module::getName() + " State", "", "switch" };
            core::mqtt::Switch    *mqtt_switch = new core::mqtt::Switch(Module::getName() + "_state", &mqtt_state, mqtt_meta);
#endif
    };
} // namespace modules