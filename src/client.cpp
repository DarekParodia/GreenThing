#include "I2C_LCD.h"
#include "client.h"
#include "core/core.h"
#include "core/time.h"
#include "core/wifi.h"
#include "modules/ds18b20.h"

#include <Arduino.h>
#include <Wire.h>
#include <string>


modules::DS18B20 co("CO", 5);
modules::DS18B20 cwu("CWU", 4);

namespace client {
    void preInit() {
        core::addModule(&co);
        core::addModule(&cwu);
    }
    void init() {
    }
    void postInit() {
    }

    void loop() {
    }

    void render() {
    }
} // namespace client