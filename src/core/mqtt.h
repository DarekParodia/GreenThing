#pragma once

#include "core/core.h"
#include "core/wifi.h"

#if defined(ESP32)
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <map>
#include <string>
#include <vector>

#define BASE_DISCOVERY "homeassistant/sensor/"

namespace core::mqtt {
    struct hass_data {
            std::string name;
            std::string unit_of_measurement;
            std::string device_class;

            hass_data() = default;

            hass_data(const std::string &name,
            const std::string           &unit_of_measurement,
            const std::string           &device_class) :
                name(name),
                unit_of_measurement(unit_of_measurement),
                device_class(device_class) {}
    };
    // this class exists only because i cannot create vector from template classes (reasonable)
    class mqtt_data_base {
        public:
            virtual void annouceHass() = 0;
    };
    template <typename T>
    class mqtt_data : mqtt_data_base {
        public:
            mqtt_data(std::string topic);
            mqtt_data(std::string topic, hass_data hdata);
            mqtt_data(std::string topic, unsigned long update_interval);
            mqtt_data(std::string topic, unsigned long update_interval, hass_data hdata);
            ~mqtt_data();
            void update(T new_data);
            void annouceHass();

        private:
            std::string   topic;
            hass_data     hdata;
            T             data;
            T             prev_data;
            unsigned long last_update     = 0;
            unsigned long update_interval = 0;
            void          publish(std::string data);
            bool          process_publish(T new_data);
    };

    extern std::string                   def_topic;
    extern std::vector<mqtt_data_base *> mqtt_bases;

    extern WiFiClient                    espClient;
    extern PubSubClient                  client;

    // functions
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
    inline mqtt_data<T>::mqtt_data(std::string topic, hass_data hdata) :
        topic(topic),
        hdata(hdata) {
        mqtt_bases.push_back(this);
    }

    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic, unsigned long update_interval) :
        topic(topic),
        update_interval(update_interval),
        hdata() {}

    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic, unsigned long update_interval, hass_data hdata) :
        topic(topic),
        update_interval(update_interval),
        hdata(hdata) {
        mqtt_bases.push_back(this);
    }

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
    inline void mqtt_data<T>::annouceHass() {
        JsonDocument doc;
        doc["name"]                = hdata.name.c_str();
        doc["state_topic"]         = (def_topic + this->topic).c_str();
        doc["unit_of_measurement"] = hdata.unit_of_measurement.c_str();
        doc["device_class"]        = hdata.device_class.c_str();
        std::string unique_id      = "gt_" + core::getChipId() + "_" + hdata.name;
        doc["unique_id"]           = unique_id.c_str();

        std::string output;
        serializeJson(doc, output);

        client.publish((std::string(BASE_DISCOVERY) + unique_id + "/config").c_str(), output.c_str());
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