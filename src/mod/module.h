#pragma once

namespace mod {
    class Module {
    public:
        Module() = default;
        virtual ~Module()= default;
        
        virtual void init() = 0;
        virtual void loop() = 0;
        void userLoop() {
            // To be overriden if needed
            this->_skipUserLoop = true;
        }

        inline bool skipUserLoop(){
            return this->_skipUserLoop;
        }

    private:
        bool _skipUserLoop = false;
    };
}