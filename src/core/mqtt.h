#pragma once

#include "core/wifi.h"

#include <WiFiManager.h>
#include <string>

namespace core::mqtt {
    // Custom parameters
    WiFiManagerParameter custom_mqtt_server("server", "MQTT Server", mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "MQTT Port", mqtt_port, 6);
    WiFiManagerParameter custom_mqtt_user("user", "MQTT User", mqtt_user, 40);
    WiFiManagerParameter custom_mqtt_pass("pass", "MQTT Password", mqtt_pass, 40);

    template <typename T>
    class mqtt_data {
        public:
            mqtt_data(std::string topic);
            ~mqtt_data();
            void update(T new_data);

        private:
            std::string topic;
            T           data;
            T           prev_data;
            void        publish(std::string data);
    };

    void setup();
    void loop();
    void reconnect();
} // namespace core::mqtt