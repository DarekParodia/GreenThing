#pragma once

#include "interface/interface.h"
#include "interface/lcd_i2c.h"

#include <Arduino.h>

#define REFRESHRATE 1;

namespace core::display{
    extern const unsigned long renderDelay;
    extern unsigned long lastRenderTime;
    extern core::display::interface::DisplayInterface *displayInterface;

    // CORE FUNCTIONS
    void setup();
    void loop();
}