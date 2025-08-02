#include "core/core.h"
#include "core/filesystem.h"
#include "core/mqtt.h"

namespace core::mqtt {
    struct mqtt_credentials {
            char     server[40] = MQTT_SERV;
            uint16_t port       = MQTT_PORT;
            char     user[40]   = MQTT_USER;
            char     pass[40]   = MQTT_PASS;
    };
    size_t           credentials_size = sizeof(mqtt_credentials);
    mqtt_credentials credentials;

    // Wifi stuff
    WiFiClient   espClient;
    PubSubClient client(espClient);

    // Custom parameters
    WiFiManagerParameter *custom_mqtt_server;
    WiFiManagerParameter *custom_mqtt_port;
    WiFiManagerParameter *custom_mqtt_user;
    WiFiManagerParameter *custom_mqtt_pass;

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
        Serial.println("Adding mqtt parameters to wifi manager");

        // read stored credentails
        core::filesystem::readFile("mqtt.bin", &credentials, &credentials_size);

        // configure parameters
        char port_str[6];
        snprintf(port_str, sizeof(port_str), "%u", credentials.port);

        custom_mqtt_server = new WiFiManagerParameter("server", "MQTT Server", credentials.server, 40);
        custom_mqtt_port   = new WiFiManagerParameter("port", "MQTT Port", port_str, 6);
        custom_mqtt_user   = new WiFiManagerParameter("user", "MQTT User", credentials.user, 40);
        custom_mqtt_pass   = new WiFiManagerParameter("pass", "MQTT Password", credentials.pass, 40);

        core::wifi::addCustomParameter(custom_mqtt_server);
        core::wifi::addCustomParameter(custom_mqtt_port);
        core::wifi::addCustomParameter(custom_mqtt_user);
        core::wifi::addCustomParameter(custom_mqtt_pass);
    }
    void init() {}
    void postInit() {
        // read parameters
        strncpy(credentials.server, custom_mqtt_server->getValue(), sizeof(credentials.server));
        credentials.port = static_cast<uint16_t>(atoi(custom_mqtt_port->getValue()));
        strncpy(credentials.user, custom_mqtt_user->getValue(), sizeof(credentials.user));
        strncpy(credentials.pass, custom_mqtt_pass->getValue(), sizeof(credentials.pass));

        // save parameters
        core::filesystem::writeFile("mqtt.bin", &credentials, &credentials_size);

        client.setServer(credentials.server, credentials.port);
        client.setCallback(mqttCallback);
        reconnect();
    }
    void loop() {
        // reconnect();
        client.loop();
    }

    void reconnect() {
        if(client.connected()) return;
        if(client.connect(core::getHostname().c_str(), credentials.user, credentials.pass)) {
            Serial.println("connected to mqtt");
        } else {
            Serial.print("failed connecting to mqtt, rc=");
            Serial.print(client.state());
        }
    }
} // namespace core::mqtt