#include "lcd_i2c.h"
#include <string>

namespace core::display::interface{
    LCD_I2C::LCD_I2C(size_t columns, size_t rows, uint16_t address){
        this->character_cols = columns;
        this->character_rows = rows;
        this->screen_width = columns * 4;
        this->screen_height = rows * 8;
        this->font_size = 8;

        this->address = address;
        this->lcd = new I2C_LCD(address);

        this->character_buffer_size = columns * rows;
        this->character_buffer = new uint8_t[this->character_buffer_size];
    }

    void LCD_I2C::init(){
        lcd->begin(this->character_cols, this->character_rows);
        lcd->clear();
    }

    void LCD_I2C::render(){
        lcd->flush();
        lcd->setCursor(0, 0);
        lcd->print(reinterpret_cast<const char*>(character_buffer));
    }
}