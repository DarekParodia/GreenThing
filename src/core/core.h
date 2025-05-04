#pragma once

#include <vector>
#include "mod/module.h"

namespace core
{
    struct timeout_t
    {
        unsigned long delay;
        void (*callback)();
        unsigned long startTime = 0;
    };

    void setup();
    void loop();
    void addModule(mod::Module *module);
    void addTimeout(timeout_t timeout);
    void removeModule(mod::Module *module);
    unsigned long getDeltaTime();
}