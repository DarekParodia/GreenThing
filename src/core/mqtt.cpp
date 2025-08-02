#include "mqtt.h"

namespace core::mqtt {
    template <typename T>
    mqtt_data<T>::mqtt_data(std::string topic) :
        topic(topic) {}

    template <typename T>
    mqtt_data<T>::~mqtt_data() {}

    template <typename T>
    void mqtt_data<T>::update(T new_data) {
        prev_data = data;
        data      = new_data;
        if(prev_data != data)
            this->publish(std::to_string(data));
    }

    template <>
    void mqtt_data<std::string>::update(std::string new_data) {
        prev_data = data;
        data      = new_data;
        if(prev_data != data)
            this->publish(data);
    }

    template <typename T>
    void mqtt_data<T>::publish(std::string data) {
    }

    void preInit() {
        core::wifi::addCustomParameter(&custom_mqtt_server);
        core::wifi::addCustomParameter(&custom_mqtt_port);
        core::wifi::addCustomParameter(&custom_mqtt_user);
        core::wifi::addCustomParameter(&custom_mqtt_pass);
    }
    void init() {}
    void postInit() {}
    void loop() {
    }
} // namespace core::mqtt