#pragma once

#include <string>
#include <ArduinoJson.h>

namespace core::api
{
    enum class DataPointType
    {
        UNDEFINED = 0,
        SOLENOID = 1,
        BUTTON = 2,
        LED = 3
    };

    class DataPoint
    {
    public:
        DataPoint(std::string name) : name(name) {};
        ~DataPoint() = default;

        virtual void addToJsonArray(JsonArray& array) = 0;
        virtual void handleGetRequest(const std::string& requestBody) = 0;
        virtual void handlePostRequest(const std::string& requestBody) = 0;

        std::string getName() const { return name; }
        DataPointType getType() const { return type; }

    protected:
        std::string name;
        DataPointType type = DataPointType::UNDEFINED;
    };
}