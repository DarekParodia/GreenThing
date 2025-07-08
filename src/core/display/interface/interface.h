#pragma once

#include <string>
#include <cstring>
#include <unistd.h>

#include <Arduino.h>

namespace core::display::interface {
    class DisplayInterface {
        public:
            DisplayInterface() = default;
            ~DisplayInterface() = default;

            virtual void init() = 0;
            virtual void render() = 0;

            inline void setText(std::string text){
                size_t buf_index = getCursorIndex();
                memcpy(character_buffer + buf_index, text.c_str(), text.size());
                moveCursor(text.size());
            }
            
            inline void setCursor(size_t x, size_t y) {
                this->cursor_x = x % character_cols;
                this->cursor_y = y % character_rows;
            }

            inline void moveCursor(size_t count){
                size_t index = getCursorIndex() + count;
                cursor_y = index / character_cols;
                cursor_x = index % character_cols;
            
                cursor_y = cursor_y % character_rows;
            }

        protected:
            // Dimensions
            size_t character_cols;
            size_t character_rows;
            size_t screen_width;
            size_t screen_height;
            uint8_t font_size;

            size_t cursor_x = 0;
            size_t cursor_y = 0;

            // Text Printing
            char *character_buffer = nullptr;
            size_t character_buffer_size;

            inline size_t getCursorIndex(){
                return (character_cols * cursor_y) + cursor_x;
            }
    };
}