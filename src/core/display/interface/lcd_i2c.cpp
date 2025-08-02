#include "core/display/display.h"
#include "lcd_i2c.h"

#include <Arduino.h>
#include <string>

namespace core::display::interface {
    LCD_I2C::LCD_I2C(size_t columns, size_t rows, uint16_t address) {
        this->character_cols        = columns;
        this->character_rows        = rows;
        this->screen_width          = columns * 5;
        this->screen_height         = rows * 8;
        this->font_size             = 8;

        this->address               = address;
        this->lcd                   = new I2C_LCD(address);

        this->character_buffer_size = columns * rows;
        this->character_buffer      = (char *) malloc(this->character_buffer_size);
        this->prev_buffer           = (char *) malloc(this->character_buffer_size);
        this->clear();
        this->setBacklight(true);

        Serial.printf("LCD_I2C initialized with %zu columns and %zu rows at address 0x%02X\n", columns, rows, address);
        Serial.printf("Screen size: %zu x %zu pixels\n", screen_width, screen_height);
        Serial.printf("Character buffer size: %zu bytes\n", character_buffer_size);
    }

    LCD_I2C::~LCD_I2C() {
        free(character_buffer);
        free(prev_buffer);
    }

    void LCD_I2C::init() {
        bool success = lcd->begin(this->character_cols, this->character_rows);
        if(!success) {
            this->init_failed = true;
            Serial.println("LCD initialization failed!");
            return;
        }
        lcd->clear();
        this->createCustomChars();
        this->welcome();
        this->render();
    }

    void LCD_I2C::render() {
        if(init_failed) return;
        Wire.setClock(100000);
        Wire.flush();
        lcd->flush();
        // lcd->clear();
        // size_t pointerIndex = 0;
        // for (size_t row = 0; row < character_rows; row++){
        //     lcd->setCursor(0, row);
        //     lcd->flush();
        //     char line[character_cols] = {0};
        //     memcpy(line, character_buffer + pointerIndex, character_cols);
        //     lcd->print(line);
        //     lcd->flush();
        //     pointerIndex += character_cols;
        // }

        // for(size_t i = 0; i < custom_char_positions.size(); i++){
        //     core::display::interface::char_pos chp = custom_char_positions[i];
        //     lcd->setCursor(chp.index % character_cols, chp.index / character_cols);
        //     lcd->write(chp.character);
        //     // Serial.print("Wrote character: ");
        //     // Serial.println(chp.character);
        // }
        // Wire.setClock(I2C_SPEED);
        // Wire.flush();

        // lcd->clear();
        size_t pointerIndex = 0;
        for(size_t row = 0; row < character_rows; row++) {
            for(size_t col = 0; col < character_cols; col++) {
                size_t bufIndex = pointerIndex + col;
                char   c        = character_buffer[bufIndex];
                char   prev_c   = prev_buffer[bufIndex];
                if(c == prev_c) continue;
                lcd->setCursor(col, row);
                lcd->write(c); // Write each character individually
                lcd->flush();
            }
            pointerIndex += character_cols;
        }

        // Render custom characters at their positions
        for(size_t i = 0; i < custom_char_positions.size(); i++) {
            core::display::interface::char_pos chp = custom_char_positions[i];
            size_t                             col = chp.index % character_cols;
            size_t                             row = chp.index / character_cols;
            lcd->setCursor(col, row);
            lcd->write(chp.character);
            lcd->flush();
        }

        char *tmp        = prev_buffer;
        prev_buffer      = character_buffer;
        character_buffer = tmp;
        this->clear();

        Wire.setClock(I2C_SPEED);
        Wire.flush();
    }

    void LCD_I2C::setBacklight(bool on) {
        if(init_failed) return;
        if(on) lcd->backlight();
        else lcd->noBacklight();
    }

    void LCD_I2C::createCustomChars() {
        if(init_failed) return;
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
} // namespace core::display::interface