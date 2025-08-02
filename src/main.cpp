#include "core/core.h"

#include <Arduino.h>

#if !defined(I2C_SPEED)
    #define I2C_SPEED 10000
#endif

#if defined(USE_MQTT)
    #if !defined(USE_WIFI) // MQTT needs wifi to work
        #define USE_WIFI
    #endif
    #include "core/mqtt.h"
#endif

#if defined(USE_WIFI)
    #include "core/wifi.h"
#endif

#if defined(USE_DISPLAY)
    #include "core/display/display.h"
#endif

#include "client.h"

void preInit();
void init();
void postInit();

void setup() {
    preInit();
    init();
    postInit();
}

void preInit() {
    Serial.begin(115200);
    Wire.setClock(I2C_SPEED);

    while(!Serial);
    Serial.println();
    Serial.println("Starting setup...");

#if defined(ESP32)
    Wire.begin(21, 22);
#elif defined(ESP8266)
    Wire.begin(4, 5);
#endif


#ifdef USE_MQTT
    core::mqtt::preInit();
#endif
}

void init() {
    core::setup();
#ifdef USE_DISPLAY
    core::display::setup();
#endif
#ifdef USE_WIFI // wifi must initialize before anything related to wifi (mqtt for example)
    core::wifi::setup();
#endif
#ifdef USE_MQTT
    core::mqtt::init();
#endif
    client::setup();
}

void postInit() {
    Wire.setClock(I2C_SPEED);

#ifdef USE_MQTT
    core::mqtt::postInit();
#endif

    Serial.println("Setup complete");
}

void loop() {
    core::loop();

#ifdef USE_WIFI
    core::wifi::loop();
#endif
#ifdef USE_MQTT
    core::mqtt ::loop();
#endif
#ifdef USE_DISPLAY
    core::display::loop();
#endif

    client::loop();
}
