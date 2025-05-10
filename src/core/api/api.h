#pragma once

#include <string>
#include <cstdint>

namespace core::api
{
    const std::string hostname = "GreenThing";
    const uint16_t webserver_port = 80;

    void setup();
    void loop();
    void enable();
    void disable();
}