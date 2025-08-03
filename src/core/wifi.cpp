#include "core/core.h"
#include "core/display/display.h"
#include "core/time.h"
#include "core/wifi.h"

#include <Arduino.h>
#if defined(USE_OTA)
    #include <ArduinoOTA.h>
#endif
namespace core::wifi {
    WiFiManager wifiManager;

// ota
#if defined(USE_OTA)
    void setupOTA() {
        ArduinoOTA.setHostname(core::getHostname().c_str());

        ArduinoOTA.onStart([]() {
            Serial.println("OTA update starting...");
            core::display::displayInterface->clear();
        });

        ArduinoOTA.onEnd([]() {
            Serial.println("\nOTA update complete!");
        });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            char buf[32];
            snprintf(buf, sizeof(buf), "OTA: %u%%", (progress * 100) / total);
            Serial.println(buf);
            core::display::displayInterface->setCursor(10, 1);
            core::display::displayInterface->setTextCentered(std::string(buf));
            core::display::loop();
        });

        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("OTA Error[%u]: ", error);
        });

        ArduinoOTA.begin();
    }
#endif

#if !defined(ESP8266)
    void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        core::syncNTP();
    }
#endif

    // Helper for common WiFiManager setup
    void setupWiFiManager() {
        wifiManager.setConfigPortalBlocking(true);
        wifiManager.setConfigPortalTimeout(0);
        Serial.println("Starting wifi manager...");
        if(!wifiManager.autoConnect(core::getHostname().c_str())) {
            // Could not connect, portal started
        } else {
            Serial.println("Connected to WiFi: " + WiFi.SSID());
#if defined(ESP8266)
            core::syncNTP();
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
#endif
        }
        wifiManager.process();
    }

    void setup() {
        WiFi.mode(WIFI_STA);
#if defined(ESP8266)
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
        WiFi.setAutoReconnect(true);
        WiFi.hostname(core::getHostname().c_str());
#else
        WiFi.onEvent(onWiFiConnected, ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.setAutoReconnect(true);
        WiFi.setAutoConnect(true);
        WiFi.setHostname(core::getHostname().c_str());
#endif
        setupWiFiManager();
#if defined(USE_OTA)
        setupOTA();
#endif
    }

    void loop() {
        wifiManager.process();
#if defined(USE_OTA)
        ArduinoOTA.handle();
#endif
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