#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include "core/api/datapoint.h"

namespace core::api
{
    const uint16_t webserver_port = 80;
    const uint16_t ota_port = 8266;

    void setup();
    void loop();
    void enable();
    void disable();
    void addDataPoint(DataPoint *dataPoint);
    std::vector<DataPoint *> getDataPoints();
}