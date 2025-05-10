#pragma once

#include <string>

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
        uint16_t port = 80;
        std::string *hostname = "GreenThing";
    };
}