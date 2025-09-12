#include "core/filesystem.h"
#include "core/mqtt.h"

namespace core::mqtt {
    struct mqtt_credentials {
            char     server[40] = MQTT_SERV;
            uint16_t port       = MQTT_PORT;
            char     user[40]   = MQTT_USER;
            char     pass[40]   = MQTT_PASS;
    };
    size_t                        credentials_size = sizeof(mqtt_credentials);
    mqtt_credentials              credentials;
    std::string                   def_topic  = "";
    std::vector<mqtt_data_base *> mqtt_bases = {};

    // Wifi stuff
    WiFiClient   espClient;
    PubSubClient client(espClient);

    // Custom parameters
    WiFiManagerParameter *custom_mqtt_server;
    WiFiManagerParameter *custom_mqtt_port;
    WiFiManagerParameter *custom_mqtt_user;
    WiFiManagerParameter *custom_mqtt_pass;

    //
    void mqttCallback(char *topic, byte *payload, unsigned int length) {
        for(int i = 0; i < mqtt_bases.size(); i++) {
            mqtt_data_base *base       = mqtt_bases[i];
            std::string     full_topic = def_topic + base->getTopic();

            if(full_topic == topic)
                base->receive(payload, length);
        }
    }

    void mqtt_publish(std::string topic, std::string data, bool retain) {
        Serial.print("Publishing mqtt: ");
        Serial.print((def_topic + topic).c_str());
        Serial.print(" | ");
        Serial.println(data.c_str());
        client.publish((def_topic + topic).c_str(), data.c_str(), retain);
    }

    std::string hass_type_to_string(HassType type) {
        auto it = HassTypeToString.find(type);
        return it != HassTypeToString.end() ? it->second : "unknown";
    }

    bool isConnected() {
        return client.connected();
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
    void init() {
        def_topic = std::string(HOSTNAME) + "/" + core::getChipId() + "/";
    }
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

        unsigned long       startAttemptTime = millis();
        const unsigned long timeout          = 5000; // 5 seconds

        while(millis() - startAttemptTime < timeout && !client.connected()) {
            reconnect();
            delay(100);
        }

        for(int i = 0; i < mqtt_bases.size(); i++)
            mqtt_bases[i]->init();
    }
    void loop() {
        static unsigned long lastRecconect = millis();
        const unsigned long  timeout       = 15000; // 15 seconds

        if(millis() - lastRecconect > timeout && !client.connected()) {
            reconnect();
            lastRecconect = millis();
        }
        client.loop();
    }

    void reconnect() {
        if(client.connected()) return;
        if(client.connect(core::getHostname().c_str(), credentials.user, credentials.pass)) {
            Serial.println("connected to mqtt");
            for(int i = 0; i < mqtt_bases.size(); i++) {
                auto base = mqtt_bases[i];
                base->init();
            }

        } else {
            Serial.print("failed connecting to mqtt, rc=");
            Serial.println(client.state());
        }
    }
} // namespace core::mqtt