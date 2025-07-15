#pragma once

#include "core/display/interface/interface.h"
#include <I2C_LCD.h>

namespace core::display::interface {
    class LCD_I2C : public DisplayInterface {
        public:
            LCD_I2C(size_t columns, size_t rows, uint16_t address = 39);
            ~LCD_I2C();

            void init() override;
            void render() override;

            void setBacklight(bool on) override;

        protected:
            // LCD Parameters
            I2C_LCD *lcd;
            uint16_t address;

            void     createCustomChars() override;
    };
} // namespace core::display::interface