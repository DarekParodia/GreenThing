#pragma once

#include <string>
#include <cstring>
#include <unistd.h>

namespace core::display::interface {
    class DisplayInterface {
        public:
            DisplayInterface();
            ~DisplayInterface();

            virtual void init();
            virtual void render();

            inline void setText(std::string text){
                size_t buf_index = getCursorIndex();
                memcpy(this->character_buffer + buf_index, text.c_str(), text.size());
                moveCursor(text.size());
            }
            
            inline void setCursor(size_t x, size_t y) {
                this->cursor_x = x;
                this->cursor_y = y;
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

            size_t cursor_x;
            size_t cursor_y;

            // Text Printing
            uint8_t *character_buffer = nullptr;
            size_t character_buffer_size;

            inline size_t getCursorIndex(){
                return character_cols * cursor_y + cursor_x;
            }
    };
}