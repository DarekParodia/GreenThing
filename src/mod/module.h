#pragma once

namespace mod {
    class Module {
    public:
        Module() = default;
        virtual ~Module()= default;
        
        virtual void init() = 0;
        virtual void loop() = 0;
        virtual void userLoop() {
            // To be overriden if needed
        }
    };
}