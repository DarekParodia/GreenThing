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
        
    };
}