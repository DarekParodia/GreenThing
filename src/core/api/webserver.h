#pragma once

#include <string>
#include <ESP8266WebServer.h>

namespace core::api
{
    class WebServer
    {
    public:
        WebServer();
        ~WebServer();
        void init();
        void loop();
        void registerPath(std::string path, HTTPMethod method, (void (*handler)(const std::string &requestBody)));

    private:
        void handleRequest((void (*handler)(const std::string &requestBody)));
    };
}