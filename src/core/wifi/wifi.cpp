#include "wifi.h"
#include "core/time.h"

#include <Arduino.h>

namespace core::wifi
{
    WiFiManager wifiManager;

    void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        core::syncNTP();
    }

    void setup()
    {
        WiFi.mode(WIFI_STA);
        WiFi.onEvent(onWiFiConnected, ARDUINO_EVENT_WIFI_STA_GOT_IP);
#if defined(ESP8266)
        WiFi.forceSleepWake();
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
        WiFi.setAutoReconnect(true);
        WiFi.setAutoConnect(true);
        WiFi.setHostname(HOSTNAME);

        wifiManager.setConfigPortalBlocking(false);
        wifiManager.setConfigPortalTimeout(0);

        if (!wifiManager.autoConnect(HOSTNAME))
        {
            Serial.println("Starting config portal...");
        }
        else
        {
            Serial.println("Connected to WiFi: " + WiFi.SSID());
        }
    }

    void loop()
    {
        wifiManager.process();
    }

    bool isConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    std::string getStringIP(){
        if(!isConnected()) return "";
        return std::string(WiFi.localIP().toString().c_str());
    }
}