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
            mqtt_data(std::string topic, unsigned long update_interval);
            ~mqtt_data();
            void update(T new_data);

        private:
            std::string   topic;
            T             data;
            T             prev_data;
            unsigned long last_update     = 0;
            unsigned long update_interval = 0;
            void          publish(std::string data);
            bool          process_publish(T new_data);
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
    inline mqtt_data<T>::mqtt_data(std::string topic, unsigned long update_interval) :
        topic(topic),
        update_interval(update_interval) {}

    template <typename T>
    inline mqtt_data<T>::~mqtt_data() {}

    template <typename T>
    inline void mqtt_data<T>::update(T new_data) {
        if(process_publish(new_data))
            this->publish(std::to_string(data));
    }

    template <>
    inline void mqtt_data<std::string>::update(std::string new_data) {
        if(process_publish(new_data))
            this->publish(data);
    }

    template <typename T>
    inline void mqtt_data<T>::publish(std::string data) {
        mqtt_publish(this->topic, data);
    }

    template <typename T>
    inline bool mqtt_data<T>::process_publish(T new_data) {
        prev_data = data;
        data      = new_data;
        if(update_interval == 0)
            return prev_data != data;
        else {
            unsigned long now = millis();
            if(now >= last_update + update_interval && prev_data != data) {
                last_update = now;
                return true;
            }
        }
        return false;
    }
} // namespace core::mqtt