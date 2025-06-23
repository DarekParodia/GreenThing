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
#include <ArduinoOTA.h>

namespace core::api
{
    unsigned long connectionTimeout = 10000; // 10 seconds
    bool enabled = false;
    WebServer *webServer = nullptr;
    WiFiManager wifiManager;
    std::vector<DataPoint *> dataPoints;

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

        if (!enabled)
        {
            Serial.println("Failed to enable API");
            return;
        }

        // Setup OTA
        ArduinoOTA.setPort(ota_port);

        ArduinoOTA.onStart([]()
                           {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
            {
                type = "sketch";
            }
            else
            {
                type = "filesystem";
            }
            Serial.println("Start updating " + type); });

        ArduinoOTA.onEnd([]()
                         {
            Serial.println("End update");
            Serial.println("Rebooting...");
            delay(1000);
            ESP.restart(); });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                              { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });

        ArduinoOTA.onError([](ota_error_t error)
                           {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
            {
                Serial.println("Auth Failed");
            }
            else if (error == OTA_BEGIN_ERROR)
            {
                Serial.println("Begin Failed");
            }
            else if (error == OTA_CONNECT_ERROR)
            {
                Serial.println("Connect Failed");
            }
            else if (error == OTA_RECEIVE_ERROR)
            {
                Serial.println("Receive Failed");
            }
            else if (error == OTA_END_ERROR)
            {
                Serial.println("End Failed");
            } });

        ArduinoOTA.begin();
        Serial.println("OTA setup complete");
    }

    void loop()
    {
        if (!enabled)
            return;

        if (!reconnect())
        {
            return;
        }

        if (webServer)
        {
            webServer->loop();
        }

        ArduinoOTA.handle();
    }

    void enable()
    {
        if (enabled)
            return;

        WiFi.mode(WIFI_STA);
#if defined(ESP8266)
        WiFi.forceSleepWake();
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
        WiFi.setAutoReconnect(true);
        WiFi.setAutoConnect(true);
        WiFi.setHostname(hostname.c_str());

        if (!reconnect())
        {
            return;
        }

        webServer = new WebServer();
        webServer->init();
        enabled = true;
    }

    void disable()
    {
        if (!enabled)
            return;
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        if (webServer)
        {
            delete webServer;
            webServer = nullptr;
        }
        enabled = false;
    }

    void addDataPoint(DataPoint *dataPoint)
    {
        dataPoints.push_back(dataPoint);
    }

    std::vector<DataPoint *> getDataPoints()
    {
        return dataPoints;
    }
}