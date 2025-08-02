#pragma once

#include <Arduino.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <vector>

namespace core::display::interface {

    enum CustomChar : uint8_t {
        CELSIUS = 0
    };
    struct char_pos {
            size_t                               index;
            core::display::interface::CustomChar character;
    };

    class DisplayInterface {
        public:
            DisplayInterface() = default;
            virtual ~DisplayInterface();

            virtual void init()                = 0;
            virtual void render()              = 0;

            virtual void setBacklight(bool on) = 0;

            inline void  welcome() {
                size_t centerx = character_cols / 2;
                size_t centery = character_rows / 2;
                setCursor(centerx, centery - 1);
                setTextCentered("Welcome!");
                setCursor(centerx, centery);
                setTextCentered(HOSTNAME);
                setCursor(0, 0);
            }

            inline void setText(std::string text) {
                size_t buf_index = getCursorIndex();
                memcpy(character_buffer + buf_index, text.c_str(), text.size());
                moveCursor(text.size());
            }

            inline void setText(int text) {
                setText(std::to_string(text));
            }

            inline void setTextCentered(std::string text) {
                moveCursor(-(text.size() / 2));
                setText(text);
            }

            inline void setCustomChar(core::display::interface::CustomChar character) {
                char_pos pos = { getCursorIndex(), character };
                custom_char_positions.push_back(pos);
                moveCursor(1);
            }

            inline void setCursor(size_t x, size_t y) {
                this->cursor_x = x % character_cols;
                this->cursor_y = y % character_rows;
            }

            inline void moveCursor(size_t count) {
                size_t index = getCursorIndex() + count;
                cursor_y     = index / character_cols;
                cursor_x     = index % character_cols;

                cursor_y     = cursor_y % character_rows;
            }

            inline void clear() {
                memset(this->character_buffer, ' ', this->character_buffer_size);
                custom_char_positions.clear();
            }

            inline size_t getCols() {
                return character_cols;
            }

            inline size_t getRows() {
                return character_rows;
            }

        protected:
            bool init_failed = false;
            // Dimensions
            size_t  character_cols;
            size_t  character_rows;
            size_t  screen_width;
            size_t  screen_height;
            uint8_t font_size;

            size_t  cursor_x                       = 0;
            size_t  cursor_y                       = 0;

            // Text Printing
            char                 *character_buffer = nullptr;
            char                 *prev_buffer      = nullptr;
            size_t                character_buffer_size;

            std::vector<char_pos> custom_char_positions;

            inline size_t         getCursorIndex() {
                return (character_cols * cursor_y) + cursor_x;
            }

            virtual void createCustomChars() {};
    };
    // Out-of-line destructor definition to ensure vtable is generated
    inline DisplayInterface::~DisplayInterface() = default;
} // namespace core::display::interface