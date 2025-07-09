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

#if defined(USE_DISPLAY)
#include "core/display/display.h"
#endif

#include "client.h"

void setup()
{
  Serial.begin(115200);
  Wire.setClock(I2C_SPEED);
  
  while (!Serial)
    ;
  Serial.println();
  Serial.println("Starting setup...");

  Wire.begin(4, 5);
  core::setup();

#ifdef USE_DISPLAY
  core::display::setup();
#endif
#ifdef USE_WIFI
  core::wifi::setup();
#endif
#ifdef USE_API
  core::api::setup();
#endif

  client::setup();

  Wire.setClock(I2C_SPEED);

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
#ifdef USE_DISPLAY
  core::display::loop();
#endif

  client::loop();
}
