#pragma once

#include <string>
#if defined(ESP32)
#include <WebServer.h>
#elif defined(ESP8266)
#include <ESP8266WebServer.h>
#endif
namespace core::api
{
    class WebServer
    {
    public:
        WebServer();
        ~WebServer();
        void init();
        void loop();
        void registerPath(std::string path, HTTPMethod method, void (*handler)(const std::string &requestBody));

    private:
        void handleRequest(void (*handler)(const std::string &requestBody));
    };
}