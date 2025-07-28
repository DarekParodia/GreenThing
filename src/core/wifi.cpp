#include "core/time.h"
#include "wifi.h"

#include <Arduino.h>

namespace core::wifi {
    WiFiManager wifiManager;

#if defined(ESP8266)
    void setup() {
        WiFi.mode(WIFI_STA);
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
        WiFi.setAutoReconnect(true);
        WiFi.hostname(HOSTNAME); // ESP8266: setHostname is 'hostname()'
        wifiManager.setConfigPortalBlocking(false);
        wifiManager.setConfigPortalTimeout(0);
        if(!wifiManager.autoConnect(HOSTNAME)) {
            Serial.println("Starting config portal...");
        } else {
            Serial.println("Connected to WiFi: " + WiFi.SSID());
            core::syncNTP();
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        }
        wifiManager.process();
    }
#else
    void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        core::syncNTP();
    }

    void setup() {
        WiFi.mode(WIFI_STA);
        WiFi.onEvent(onWiFiConnected, ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.setAutoReconnect(true);
        WiFi.setAutoConnect(true);
        WiFi.setHostname(HOSTNAME);
        wifiManager.setConfigPortalBlocking(false);
        wifiManager.setConfigPortalTimeout(0);
        if(!wifiManager.autoConnect(HOSTNAME))
            Serial.println("Starting config portal...");
        else
            Serial.println("Connected to WiFi: " + WiFi.SSID());
        wifiManager.process();
    }
#endif

    void loop() {
        wifiManager.process();
    }

    void addCustomParameter(WiFiManagerParameter *parameter) {
        wifiManager.addParameter(parameter);
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

    std::string getStringIP() {
        if(!isConnected()) return "";
        return std::string(WiFi.localIP().toString().c_str());
    }
} // namespace core::wifi