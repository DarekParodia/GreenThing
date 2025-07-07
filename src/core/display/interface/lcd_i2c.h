#pragma once

#include <I2C_LCD.h>

#include "core/display/interface/interface.h"

namespace core::display::interface{
    class LCD_I2C : public DisplayInterface {
        public:
            LCD_I2C(size_t columns, size_t rows, uint16_t address = 39);
            ~LCD_I2C();

            void init() override;
            void render() override;
    
        protected:
            // Screen Parameters
            size_t character_cols;
            size_t character_rows;

            size_t screen_width;
            size_t screen_height;

            uint8_t font_size;

            // LCD Parameters
            I2C_LCD *lcd;
            uint16_t address;
    };
}