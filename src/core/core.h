#pragma once

#include <vector>
#include "mod/module.h"

namespace core
{
    const int loopTime = 10000; // Delay in microseconds

    void setup();
    void loop();
    void addModule(mod::Module *module);
    void removeModule(mod::Module *module);
    unsigned long getDeltaTime();
}