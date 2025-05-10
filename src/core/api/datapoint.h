#pragma once

#include <string>
#include <ArduinoJson.h>

namespace core::api
{
    enum class DataPointType
    {
        UNDEFINED,
        SOLENOID,
        BUTTON,
        LED
    };

    class DataPoint
    {
    public:
        DataPoint(std::string name) : name(name) {};
        ~DataPoint() = default;

        virtual JsonObject toJSON() = 0;

        std::string getName() const { return name; }
        DataPointType getType() const { return type; }

    protected:
        std::string name;
        DataPointType type = DataPointType::UNDEFINED;
    };
}