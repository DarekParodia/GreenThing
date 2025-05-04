#pragma once

#include <vector>
#include "mod/module.h"

namespace core
{
    // Constants
    const int loopTime = 100; // Delay in milliseconds

    // Global variables
    unsigned long lastLoopTime = 0;
    unsigned long currentLoopTime = 0;
    unsigned long deltaTime = 0;
}