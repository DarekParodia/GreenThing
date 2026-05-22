#include "core/wifi.h"

#include "core/core.h"
#include "core/display/display.h"
#include "core/time.h"

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
        wifiManager.setConfigPortalTimeout(180); // 3 minute timeout for config portal
        wifiManager.setConnectTimeout(30);       // 30 second timeout for connection attempts

        Serial.println("Starting WiFi Manager...");

        // Blocking autoConnect - will open config portal if no saved credentials
        if(!wifiManager.autoConnect(core::getHostname().c_str())) {
            Serial.println("Failed to connect to WiFi, continuing offline...");
        } else {
            Serial.println("Connected to WiFi: " + WiFi.SSID());
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
#if defined(ESP8266)
            core::syncNTP();
#endif
        }
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

        // Actively monitor WiFi and attempt reconnection with exponential backoff
        static unsigned long lastReconnectAttempt = 0;
        static int           reconnectAttempts    = 0;
        const unsigned long  backoffTimes[]       = { 1000, 5000, 15000, 30000, 60000 }; // progressive backoff
        const int            maxRetries           = 5;

        if(WiFi.status() != WL_CONNECTED) {
            unsigned long backoff = backoffTimes[min(reconnectAttempts, maxRetries - 1)];

            if(millis() - lastReconnectAttempt > backoff) {
                Serial.print("WiFi reconnection attempt ");
                Serial.println(reconnectAttempts + 1);
                WiFi.reconnect();
                lastReconnectAttempt = millis();
                reconnectAttempts++;
            }
        } else if(reconnectAttempts > 0) {
            reconnectAttempts = 0; // Reset on successful connection
        }

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