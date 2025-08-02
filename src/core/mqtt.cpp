#include "core/core.h"
#include "core/mqtt.h"

namespace core::mqtt {
    const char *mqtt_serv;
    const char *mqtt_port;
    const char *mqtt_user;
    const char *mqtt_pass;

    // Wifi stuff
    WiFiClient   espClient;
    PubSubClient client(espClient);

    // Custom parameters
    WiFiManagerParameter custom_mqtt_server("server", "MQTT Server", MQTT_SERV, 40);
    WiFiManagerParameter custom_mqtt_port("port", "MQTT Port", MQTT_PORT, 6);
    WiFiManagerParameter custom_mqtt_user("user", "MQTT User", MQTT_USER, 40);
    WiFiManagerParameter custom_mqtt_pass("pass", "MQTT Password", MQTT_PASS, 40);

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

    void mqttCallback(char *topic, byte *payload, unsigned int length) {
    }

    void preInit() {
        core::wifi::addCustomParameter(&custom_mqtt_server);
        core::wifi::addCustomParameter(&custom_mqtt_port);
        core::wifi::addCustomParameter(&custom_mqtt_user);
        core::wifi::addCustomParameter(&custom_mqtt_pass);
    }
    void init() {}
    void postInit() {
        mqtt_serv = custom_mqtt_server.getValue();
        mqtt_port = custom_mqtt_port.getValue();
        mqtt_user = custom_mqtt_user.getValue();
        mqtt_pass = custom_mqtt_pass.getValue();

        client.setServer(mqtt_serv, atoi(mqtt_port));
        client.setCallback(mqttCallback);
        reconnect();
    }
    void loop() {
        // reconnect();
        client.loop();
    }

    void reconnect() {
        if(client.connected()) return;
        if(client.connect(core::getHostname().c_str(), mqtt_user, mqtt_pass)) {
            Serial.println("connected to mqtt");
        } else {
            Serial.print("failed connecting to mqtt, rc=");
            Serial.print(client.state());
        }
    }
} // namespace core::mqtt