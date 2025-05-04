#pragma once

#include <vector>
#include "mod/module.h"

namespace core
{
    struct timeout_t
    {
        unsigned long delay;
        unsigned long startTime;
        void (*callback)(void *custom_pointer);
        void *custom_pointer = nullptr; // Pointer to custom data
    };
    typedef struct timeout_t timeout_t;

    void setup();
    void loop();
    void addModule(mod::Module *module);
    void addTimeout(timeout_t *timeout);
    void removeModule(mod::Module *module);
    unsigned long getDeltaTime();
}