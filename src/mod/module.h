#pragma once

namespace mod {
    class Module {
    public:
        Module() = default;
        ~Module()= default;
        
        virtual void init();
        virtual void loop();
    };
}