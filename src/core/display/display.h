#pragma once

#include "core/display/interface/interface.h"
#include "core/display/interface/lcd_i2c.h"

#include <Arduino.h>

#define REFRESHRATE 2

namespace core::display{
    extern const unsigned long renderDelay;
    extern unsigned long lastRenderTime;
    extern unsigned long frameCount;
    extern core::display::interface::DisplayInterface *displayInterface;

    // CORE FUNCTIONS
    void setup();
    void loop();
}