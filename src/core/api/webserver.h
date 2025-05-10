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

    private:

    };
}