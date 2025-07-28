#pragma once

#include <string>

#if defined(ESP32)
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#else
    #error "Unsupported platform, please use ESP32 or ESP8266"
#endif

#include <WiFiManager.h>

namespace core::wifi {
    void        setup();
    void        loop();

    void        addCustomParameter(WiFiManagerParameter *parameter);
    bool        isConnected();

    std::string getStringIP();
} // namespace core::wifi