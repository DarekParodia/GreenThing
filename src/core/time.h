#pragma once

#include <ESP32Time.h>

namespace core
{
    extern const char *ntpServer;
    extern const long gmtOffset_sec; 
    extern const int daylightOffset_sec;
    extern const char* timezone;

    extern ESP32Time *time;

    bool syncNTP();
}