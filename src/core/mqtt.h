#pragma once

#include "core/core.h"
#include "core/wifi.h"

#if defined(ESP32)
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

#include <unordered_map>

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <map>
#include <string>
#include <vector>

#define BASE_DISCOVERY "homeassistant/"

namespace core::mqtt {
    enum class HassType {
        BUTTON,
        NUMBER,
        SWITCH,
        SENSOR,
        LIGHT,
        BINARY_SENSOR
    };
    struct hass_data {
            HassType    type = HassType::SENSOR;
            std::string name;
            std::string unit_of_measurement = "";
            std::string device_class        = "";

            hass_data()                     = default;

            hass_data(const std::string &name,
            const std::string           &unit_of_measurement = "",
            const std::string           &device_class        = "") :
                name(name),
                unit_of_measurement(unit_of_measurement),
                device_class(device_class) {}

            hass_data(HassType type,
            const std::string &name,
            const std::string &unit_of_measurement = "",
            const std::string &device_class        = "") :
                type(type),
                name(name),
                unit_of_measurement(unit_of_measurement),
                device_class(device_class) {}
    };
    // this class exists only because i cannot create vector from template classes (reasonable)
    class mqtt_data_base {
        public:
            virtual void        init()                                      = 0;
            virtual void        receive(byte *payload, unsigned int length) = 0;
            virtual std::string getTopic()                                  = 0;
    };
    template <typename T>
    class mqtt_data : mqtt_data_base {
        public:
            mqtt_data(std::string topic);
            mqtt_data(std::string topic, hass_data hdata);
            mqtt_data(std::string topic, unsigned long update_interval);
            mqtt_data(std::string topic, unsigned long update_interval, hass_data hdata);
            mqtt_data(std::string topic, T *update_data); // I should start adding comments because it becomes confusing which constructor does what
            mqtt_data(std::string topic, T *update_data, unsigned long update_interval);
            mqtt_data(std::string topic, T *update_data, hass_data hdata);
            mqtt_data(std::string topic, T *update_data, unsigned long update_interval, hass_data hdata);
            ~mqtt_data();
            void        update(T new_data, bool retain = false);
            void        update(bool retain = false);
            void        init();
            void        receive(byte *payload, unsigned int length);
            std::string getTopic();

        private:
            std::string   topic;
            hass_data     hdata;
            T             data;
            T             prev_data;
            T            *update_data     = nullptr;
            unsigned long last_update     = 0;
            unsigned long update_interval = 0;
            bool          hass            = false;
            bool          initialized     = false;
            void          publish(std::string data, bool retain);
            void          subscribe();
            bool          process_publish(T new_data);
            T             convertFromString(const std::string &str);
    };

    extern std::string                              def_topic;
    extern std::vector<mqtt_data_base *>            mqtt_bases;

    extern WiFiClient                               espClient;
    extern PubSubClient                             client;

    const std::unordered_map<HassType, std::string> HassTypeToString = {
        {        HassType::BUTTON,        "button" },
        {        HassType::NUMBER,        "number" },
        {        HassType::SWITCH,        "switch" },
        {        HassType::SENSOR,        "sensor" },
        {         HassType::LIGHT,         "light" },
        { HassType::BINARY_SENSOR, "binary_sensor" }
    };

    // functions
    void        mqtt_publish(std::string topic, std::string data, bool retain);
    void        mqtt_subscribe();
    std::string hass_type_to_string(HassType type);
    bool        isConnected();

    void        preInit();
    void        init();
    void        postInit();
    void        loop();
    void        reconnect();

    // Class definition (linker didn't liked that it was in .cpp file)
    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic) :
        topic(topic) {
        mqtt_bases.push_back(this);
    }

    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic, hass_data hdata) :
        topic(topic),
        hdata(hdata) {
        this->hass = true;
        mqtt_bases.push_back(this);
    }

    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic, unsigned long update_interval) :
        topic(topic),
        update_interval(update_interval),
        hdata() {
        mqtt_bases.push_back(this);
    }

    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic, unsigned long update_interval, hass_data hdata) :
        topic(topic),
        update_interval(update_interval),
        hdata(hdata) {
        this->hass = true;
        mqtt_bases.push_back(this);
    }

    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic, T *update_data) :
        topic(topic),
        update_data(update_data) {
        mqtt_bases.push_back(this);
    }

    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic, T *update_data, unsigned long update_interval) :
        topic(topic),
        update_data(update_data),
        update_interval(update_interval) {
        mqtt_bases.push_back(this);
    }

    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic, T *update_data, hass_data hdata) :
        topic(topic),
        update_data(update_data),
        hdata(hdata) {
        this->hass = true;
        mqtt_bases.push_back(this);
    }

    template <typename T>
    inline mqtt_data<T>::mqtt_data(std::string topic, T *update_data, unsigned long update_interval, hass_data hdata) :
        topic(topic),
        update_data(update_data),
        update_interval(update_interval),
        hdata(hdata) {
        this->hass = true;
        mqtt_bases.push_back(this);
    }

    template <typename T>
    inline mqtt_data<T>::~mqtt_data() {}

    template <typename T>
    inline void mqtt_data<T>::update(T new_data, bool retain) {
        if(process_publish(new_data))
            this->publish(std::to_string(data), retain);
    }

    template <>
    inline void mqtt_data<std::string>::update(std::string new_data, bool retain) {
        if(process_publish(new_data))
            this->publish(data, retain);
    }

    template <typename T>
    inline void mqtt_data<T>::update(bool retain) {
        if(this->update_data == nullptr) return;
        T new_data = *this->update_data;
        if(process_publish(new_data))
            this->publish(std::to_string(data), retain);
    }
    template <>
    inline void mqtt_data<std::string>::update(bool retain) {
        if(this->update_data == nullptr) return;
        std::string new_data = *this->update_data;
        if(process_publish(new_data))
            this->publish(data, retain);
    }

    template <typename T>
    inline void mqtt_data<T>::init() {
        if(hass) {
            std::string output;
            std::string unique_id = "gt_" + core::getChipId() + "_" + hdata.name;

            if(this->update_data == nullptr) {
                JsonDocument doc;
                doc["name"]                = hdata.name.c_str();
                doc["state_topic"]         = (def_topic + this->topic).c_str();
                doc["unit_of_measurement"] = hdata.unit_of_measurement.c_str();
                doc["device_class"]        = hdata.device_class.c_str();
                doc["unique_id"]           = unique_id.c_str();

                serializeJson(doc, output);
            } else {
                JsonDocument doc;
                doc["name"]          = hdata.name.c_str();
                doc["command_topic"] = (def_topic + this->topic).c_str();
                doc["state_topic"]   = (def_topic + this->topic).c_str();
                doc["device_class"]  = hdata.device_class.c_str();
                if(hdata.type == HassType::SWITCH) {
                    doc["payload_on"]  = "1";
                    doc["payload_off"] = "0";
                }
                doc["unique_id"] = unique_id.c_str();

                serializeJson(doc, output);
            }
            client.publish((std::string(BASE_DISCOVERY) + hass_type_to_string(hdata.type) + "/" + unique_id + "/config").c_str(), output.c_str(), true);
            Serial.printf("Register hass at: %s\n With payload: %s\n", (std::string(BASE_DISCOVERY) + hass_type_to_string(hdata.type) + "/" + unique_id + "/config").c_str(), output.c_str());
        }
        // this->publish(std::string(""), false);
        this->subscribe();
        initialized = true;
    }

    template <typename T>
    void mqtt_data<T>::receive(byte *payload, unsigned int length) {
        if(this->update_data == nullptr) return;
        const std::string received(reinterpret_cast<const char *>(payload), length);
        Serial.printf("Topic: [%s] received: %s\n", this->topic.c_str(), received.c_str());
        *this->update_data = this->convertFromString(received);
    }

    template <typename T>
    inline std::string mqtt_data<T>::getTopic() {
        return this->topic;
    }

    template <typename T>
    inline void mqtt_data<T>::publish(std::string data, bool retain) {
        mqtt_publish(this->topic, data, retain);
    }
    template <typename T>
    inline void mqtt_data<T>::subscribe() {
        if(initialized || this->update_data == nullptr) return;
        client.subscribe((def_topic + this->topic).c_str());
    }

    template <typename T>
    inline bool mqtt_data<T>::process_publish(T new_data) {
        static constexpr double EPSILON = 0.0001; // tolerance for floats

        // Always publish the very first value
        if(!initialized) {
            data        = new_data;
            prev_data   = new_data;
            initialized = true;
            last_update = millis();
            return true;
        }

        bool changed;
        if constexpr(std::is_floating_point_v<T>) {
            // Compare floats with tolerance
            changed = fabs(new_data - data) > EPSILON;
        } else {
            // Normal comparison for non-floats
            changed = (new_data != data);
        }

        if(update_interval == 0) {
            if(changed) {
                prev_data = data;
                data      = new_data;
                return true;
            }
        } else {
            unsigned long now = millis();
            if(now >= last_update + update_interval && changed) {
                prev_data   = data;
                data        = new_data;
                last_update = now;
                return true;
            }
        }

        return false;
    }

    template <typename T>
    inline T mqtt_data<T>::convertFromString(const std::string &str) {
        if constexpr(std::is_same_v<T, int>)
            return std::stoi(str);
        else if constexpr(std::is_same_v<T, long>)
            return std::stol(str);
        else if constexpr(std::is_same_v<T, float>)
            return std::stof(str);
        else if constexpr(std::is_same_v<T, double>)
            return std::stod(str);
        else if constexpr(std::is_same_v<T, std::string>)
            return str;
    }
} // namespace core::mqtt