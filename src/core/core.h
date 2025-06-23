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

    extern const int loopTime; // Delay in microseconds
    extern const double loopsPerSecond; // Number of loops per second
    extern const double loopsPerMillisecond; // Number of loops per millisecond
    extern unsigned long loopCount; // Loop count

    // User Loop
    extern const int userLoopTime; // Delay in microseconds
    extern const double userLoopsPerSecond; // Number of loops per second
    extern const double userLoopsPerMillisecond; // Number of loops per millisecond
    extern unsigned long userLoopCount; // Loop count

    void setup();
    void loop();
    void userLoop();
    void delayMicroseconds(unsigned long delay);
    void delay(unsigned long delay);
    void addModule(mod::Module *module);
    void addTimeout(timeout_t *timeout);
    void removeModule(mod::Module *module);
    unsigned long getDeltaTime();
    std::vector<mod::Module *> getModules();
}