#pragma once
#include <string>

namespace core::api
{
    enum class DataType
    {
        SOLENOID,
        BUTTON
    }

    class Data
    {
        public:
            virtual Data() = default;
            ~Data() = default;

            virtual std::string toJSON() = 0;
    }
}