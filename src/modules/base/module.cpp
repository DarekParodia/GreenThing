#include "modules/base/module.h"

#include <Arduino.h>
#include <string>

namespace modules {
    Module::Module(std::string name) {
        this->name = name;
    }

    Module::Module(std::string name, unsigned long update_frequency) {
        this->name             = name;
        this->update_frequency = update_frequency;
    }

    Module::~Module() {
    }

    // Getters
    std::string Module::getName() {
        return this->name;
    }

    unsigned long Module::getUpdateFrequency() {
        return this->update_frequency;
    }

    unsigned long Module::getLoopDelta() {
        return this->loopDelta;
    }

    unsigned long Module::getUserLoopDelta() {
        return this->userLoopDelta;
    }

    // Loop Delta Calculations
    void Module::startLoop() {
        this->loopStart = micros();
    }

    void Module::endLoop() {
        this->loopDelta = micros() - this->loopStart;
    }

    // User Loop Delta Calculations
    void Module::startUserLoop() {
        this->userLoopStart = micros();
    }

    void Module::endUserLoop() {
        this->userLoopDelta = micros() - this->userLoopStart;
    }


} // namespace modules