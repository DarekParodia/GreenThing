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
    bool initialized = false;
    WebServer *server = nullptr;

    void reconnect()
    {
        // return if already connected
        if (WiFi.status() == WL_CONNECTED)
            return;

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
            Serial.println("Connected to WiFi");
        }
        else
        {
            Serial.println("Failed to connect to WiFi");
        }
    }

    void init()
    {
        if (initialized)
            return;

        WiFi.mode(WIFI_STA);
        reconnect();


        initialized = true;
    }

    void loop()
    {
    }

    void enable()
    {
    }

    void disable()
    {
    }
}