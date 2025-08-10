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
            int           mqtt_state;
#ifdef USE_MQTT
            core::mqtt::hass_data       hd        = { core::mqtt::HassType::SWITCH, Module::getName() + "_state", "", "switch" };
            core::mqtt::mqtt_data<int> *mqtt_data = new core::mqtt::mqtt_data<int>(Module::getName() + "/state", &mqtt_state, hd);
#endif
    };
} // namespace modules