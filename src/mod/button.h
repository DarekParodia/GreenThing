#pragma once

#include "mod/module.h"

namespace mod
{
    class Button : public Module
    {
    public:
        Button(int pin, bool inverted = false);
        ~Button();

        void init() override;
        void loop() override;
        bool isPressed() const;

    private:
        int pin;
        bool state;
        bool inverted;
    };
}