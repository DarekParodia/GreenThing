#pragma once

#include "modules/base/module.h"

#include <vector>

namespace core {
    struct timeout_t {
            unsigned long delay;
            unsigned long startTime;
            void (*callback)(void *custom_pointer);
            void *custom_pointer = nullptr; // Pointer to custom data
    };
    typedef struct timeout_t timeout_t;

    extern const int         loopTime;            // Delay in microseconds
    extern const double      loopsPerSecond;      // Number of loops per second
    extern const double      loopsPerMillisecond; // Number of loops per millisecond
    extern unsigned long     loopCount;           // Loop count
    extern unsigned long     deltaTime;

    // User Loop
    extern const int               userLoopTime;            // Delay in microseconds
    extern const double            userLoopsPerSecond;      // Number of loops per second
    extern const double            userLoopsPerMillisecond; // Number of loops per millisecond
    extern unsigned long           userLoopCount;           // Loop count
    extern unsigned long           userDeltaTime;

    void                           setup();
    void                           loop();
    void                           userLoop();
    void                           delayMicroseconds(unsigned long delay);
    void                           delay(unsigned long delay);
    void                           addModule(modules::Module *module);
    void                           addTimeout(timeout_t *timeout);
    void                           removeTimeout(timeout_t *timeout);
    void                           removeModule(modules::Module *module);
    unsigned long                  getDeltaTime();
    std::vector<modules::Module *> getModules();
    std::string                    getHostname();
} // namespace core