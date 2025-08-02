#pragma once

#include "core/wifi.h"


#if defined(ESP32)
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

#include <PubSubClient.h>
#include <WiFiManager.h>
#include <string>

namespace core::mqtt {
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

    void preInit();
    void init();
    void postInit();
    void loop();
    void reconnect();
} // namespace core::mqtt