#include "lcd_i2c.h"

#include "core/display/display.h"

#include <string>
#include <Arduino.h>

namespace core::display::interface{
    LCD_I2C::LCD_I2C(size_t columns, size_t rows, uint16_t address){
        this->character_cols = columns;
        this->character_rows = rows;
        this->screen_width = columns * 5;
        this->screen_height = rows * 8;
        this->font_size = 8;

        this->address = address;
        this->lcd = new I2C_LCD(address);

        this->character_buffer_size = columns * rows;
        this->character_buffer = (char *) malloc(this->character_buffer_size);
        this->clear();

        Serial.printf("LCD_I2C initialized with %zu columns and %zu rows at address 0x%02X\n", columns, rows, address);
        Serial.printf("Screen size: %zu x %zu pixels\n", screen_width, screen_height);
        Serial.printf("Character buffer size: %zu bytes\n", character_buffer_size);
    }

    LCD_I2C::~LCD_I2C(){
        free(character_buffer);
    }

    void LCD_I2C::init(){
        lcd->begin(this->character_cols, this->character_rows);
        lcd->clear();
        this->createCustomChars();
        this->welcome();
        this->render();
    }

    void LCD_I2C::render(){
        lcd->flush();
        // lcd->clear();
        size_t pointerIndex = 0;
        for (size_t row = 0; row < character_rows; row++){
            lcd->setCursor(0, row);
            char line[character_cols] = {0};
            memcpy(line, character_buffer + pointerIndex, character_cols);
            lcd->print(line);
            pointerIndex += character_cols;
        }

        for(size_t i = 0; i < custom_char_positions.size(); i++){
            core::display::interface::char_pos chp = custom_char_positions[i];
            lcd->setCursor(chp.index % character_cols, chp.index / character_cols);
            lcd->write(chp.character);
            Serial.print("Wrote character: ");
            Serial.println(chp.character);
        }
    }

    void LCD_I2C::createCustomChars(){
        // degree
        byte customChar[] = {
            B11000,
            B11000,
            B00000,
            B00111,
            B00100,
            B00100,
            B00100,
            B00111
        };

        lcd->createChar(core::display::interface::CustomChar::CELSIUS, customChar);

    }
}