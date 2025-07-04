#include <Arduino.h>

#include "core/core.h"
#if defined(USE_API)
#include "core/api/api.h"
#endif
#if defined(USE_WIFI) or defined(USE_API)
#ifndef USE_WIFI
#define USE_WIFI
#endif
#include "core/wifi/wifi.h"
#endif
#include "client.h"

// user loop TEMPORARY SOLUTION
const int userLoopTime = 1000000; // Delay in microseconds
unsigned long lastUserLoopTime = 0;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println();
  Serial.println("Starting setup...");
  core::setup();
#ifdef USE_WIFI
  core::wifi::setup();
#endif
#ifdef USE_API
  core::api::setup();
#endif
  client::setup();
  Serial.println("Setup complete");
}

void loop()
{
  core::loop();
#ifdef USE_WIFI
  core::wifi::loop();
#endif
#ifdef USE_API
  core::api::loop();
#endif
  client::loop();
  // Uncomment the following line to print the delta time
  // Serial.print("Delta Time: ");
  // Serial.println(core::getDeltaTime());
  if (micros() >= userLoopTime + lastUserLoopTime){
    client::userLoop();
    lastUserLoopTime = micros();
  }
}
