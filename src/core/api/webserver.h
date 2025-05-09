#pragma once

#include <string>

namespace core::api
{
    class WebServer
    {
    public:
        WebServer();
        ~WebServer();

    private:
        uint16_t port = 80;
        std::string *hostname = "GreenThing";
    };
}