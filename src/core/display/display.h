#pragma once

#include "interface/interface.h"
#include "interface/lcd_i2c.h"

namespace core::display{
    core::display::interface::DisplayInterface *displayInterface = nullptr;

    // CORE FUNCTIONS
    inline void setup(){
        displayInterface = new LCD_I2C(20, 4);
        displayInterface->init();
    };
    inline void render(){
        displayInterface->render();
    };
}