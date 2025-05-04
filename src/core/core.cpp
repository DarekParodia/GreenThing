#include "core/core.h"

#include <Arduino.h>

namespace core
{
    static std::vector<mod::Module *> modules;
    static std::vector<timeout_t *> timeouts;

    const int loopTime = 10000; // Delay in microseconds
    unsigned long lastLoopTime = 0;
    unsigned long currentLoopTime = 0;
    unsigned long deltaTime = 0; // Time in microseconds since the last loop

    void calculateDeltaTime()
    {
        currentLoopTime = micros();
        deltaTime = currentLoopTime - lastLoopTime;

        // Ensure the loop time is consistent
        if (deltaTime < loopTime)
        {
            delayMicroseconds(loopTime - deltaTime);
        }
        lastLoopTime = micros();
    }

    void setup()
    {
        // todo
    }

    void loop()
    {
        calculateDeltaTime();
        for (auto &module : modules)
        {
            module->loop();
        }
    }
    void addModule(mod::Module *module)
    {
        modules.push_back(module);
        module->init();
    }

    void addTimeout(timeout_t *timeout)
    {
        timeouts.push_back(timeout);
        timeout->startTime = micros();
    }

    void removeModule(mod::Module *module)
    {
        modules.erase(std::remove(modules.begin(), modules.end(), module), modules.end());
    }

    unsigned long getDeltaTime()
    {
        return deltaTime;
    }
}