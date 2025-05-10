#include "core/core.h"

#include <Arduino.h>

namespace core
{
    static std::vector<mod::Module *> modules;
    static std::vector<timeout_t *> timeouts;

    const int loopTime = 1000; // Delay in microseconds
    const double loopsPerSecond = 1000000.0 / (double)core::loopTime; // Number of loops per second
    const double loopsPerMillisecond = 1000.0 / (double)core::loopTime; // Number of loops per millisecond
    unsigned long loopCount = 0; // Loop count
    
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

    void executeTimeouts(int maxExecutionTime = -1)
    {
        unsigned long startTime = micros();
        for (auto &timeout : timeouts)
        {
            unsigned long currentTime = micros();
            if(maxExecutionTime > 0 && (currentTime - startTime) >= static_cast<unsigned long>(maxExecutionTime))
            {
                break; // Exit if the maximum execution time is reached
            }
            if (currentTime - timeout->startTime * 1000 >= timeout->delay * 1000)
            {
                timeout->callback(timeout->custom_pointer);
                timeouts.erase(std::remove(timeouts.begin(), timeouts.end(), timeout), timeouts.end());
            }
        }
    }

    void setup()
    {
        // todo
    }

    void loop()
    {
        loopCount++;
        calculateDeltaTime();
        executeTimeouts();
        for (auto &module : modules)
        {
            module->loop();
        }
    }
    void delayMicroseconds(unsigned long delay)
    {
        unsigned long startTime = micros();
        executeTimeouts(delay);
        while (micros() - startTime < delay)
        {
            // Wait for the specified delay
        }
    }
    void delay(unsigned long delay)
    {
        unsigned long startTime = micros();
        executeTimeouts(delay * 1000);
        while (micros() - startTime < delay * 1000)
        {
            // Wait for the specified delay
        }
    }
    void addModule(mod::Module *module)
    {
        modules.push_back(module);
        module->init();
    }

    void addTimeout(timeout_t *timeout)
    {
        timeout->startTime = millis();
        timeouts.push_back(timeout);
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