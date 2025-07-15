#pragma once

#include <string>

namespace modules {
    class Module {
        public:
            Module(std::string name);
            Module(std::string name, unsigned long update_frequency);
            ~Module();

            // Core Loop Functions
            virtual void init()     = 0;
            virtual void loop()     = 0;
            virtual void userLoop() = 0;

            // Getters
            std::string   getName();
            unsigned long getUpdateFrequency();
            unsigned long getLoopDelta();
            unsigned long getUserLoopDelta();

            // Loop Delta Calculations
            void startLoop();
            void endLoop();

            // User Loop Delta Calculations
            void startUserLoop();
            void endUserLoop();

        private:
            std::string   name             = "";
            unsigned long update_frequency = 0; // it's a delay (in microseconds) not frequency

            // Loop Delta
            unsigned long loopDelta        = 0;
            unsigned long loopStart        = 0;

            // User Loop Delta
            unsigned long userLoopDelta    = 0;
            unsigned long userLoopStart    = 0;
    };
} // namespace modules