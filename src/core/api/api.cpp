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

namespace core::api
{
    unsigned long connectionTimeout = 10000; // 10 seconds
    std::string *ssid = nullptr;
    std::string *password = nullptr;
    bool enabled = false;
    WebServer *server = nullptr;

    bool reconnect()
    {
        // return if already connected
        if (WiFi.status() == WL_CONNECTED)
            return true;

        Serial.println("Connecting to WiFi..");

        WiFi.begin(ssid->c_str(), password->c_str());
        unsigned long startTime = millis();

        while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < connectionTimeout)
        {
            core::delay(500);
            Serial.println(".");
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        return false;
    }

    void init()
    {
        enable();
    }

    void loop()
    {
        if (!enabled)
            return;

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi disconnected, trying to reconnect...");
            if (!reconnect())
            {
                Serial.println("Failed to reconnect to WiFi");
                return;
            }
            else>nected to WiFi");
            }
        }

        if (server)
        {
            server->loop();
        }
        else
        {
            Serial.println("Server not initialized");
        }
    }

    void enable()
    {
        if (enabled)
            return;
        
        WiFi.mode(WIFI_STA);

        if (!reconnect())
        {
            Serial.println("Failed to connect to WiFi");
            return;
        }

        Serial.println("Connected to WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("MAC address: ");
        Serial.println(WiFi.macAddress());

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
            server->stop();
            delete server;
            server = nullptr;
        }
        enabled = false;
    }
}