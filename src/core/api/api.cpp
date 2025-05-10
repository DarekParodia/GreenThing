#include "api.h"
#include "webserver.h"
#include "core/core.h"
#include <string>

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#error "Unsupported platform, please use ESP32 or ESP8266"
#endif
#include <WiFiManager.h>

namespace core::api
{
    unsigned long connectionTimeout = 10000; // 10 seconds
    bool enabled = false;
    WebServer *server = nullptr;
    WiFiManager wifiManager;

    bool reconnect()
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi disconnected, trying to reconnect...");
            wifiManager.autoConnect(hostname.c_str());

            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.println("Reconnected to WiFi");
                Serial.print("IP address: ");
                Serial.println(WiFi.localIP());
                Serial.print("MAC address: ");
                Serial.println(WiFi.macAddress());
                return true;
            }
            else
            {
                Serial.println("Failed to reconnect to WiFi");
                return false;
            }
        }
        return true;
    }

    void setup()
    {
        enable();
    }

    void loop()
    {
        if (!enabled)
            return;

        if (!reconnect())
        {
            return;
        }

        if (server)
        {
            server->loop();
        }
    }

    void enable()
    {
        if (enabled)
            return;

        WiFi.mode(WIFI_STA);
        WiFi.forceSleepWake();
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
        WiFi.setAutoReconnect(true);
        WiFi.setAutoConnect(true);
        WiFi.setHostname(hostname.c_str());

        if (!reconnect())
        {
            return;
        }

        server = new WebServer();
        server->init();
        enabled = true;
    }

    void disable()
    {
        if (!enabled)
            return;
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        if (server)
        {
            delete server;
            server = nullptr;
        }
        enabled = false;
    }
}