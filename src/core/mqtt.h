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
#include <vector>

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

    void mqtt_publish(std::string topic, std::string data);

    void preInit();
    void init();
    void postInit();
    void loop();
    void reconnect();

    // Class definition (linker didn't liked that it was in .cpp file)
    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic) :
        topic(topic) {}

    template <typename T>
    inline mqtt_data<T>::~mqtt_data() {}

    template <typename T>
    inline void mqtt_data<T>::update(T new_data) {
        prev_data = data;
        data      = new_data;
        if(prev_data != data)
            this->publish(std::to_string(data));
    }

    template <>
    inline void mqtt_data<std::string>::update(std::string new_data) {
        prev_data = data;
        data      = new_data;
        if(prev_data != data)
            this->publish(data);
    }

    template <typename T>
    inline void mqtt_data<T>::publish(std::string data) {
        mqtt_publish(this->topic, data);
    }
} // namespace core::mqtt