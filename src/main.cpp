#include <Arduino.h>

#include "core/core.h"
#include "core/api/api.h"
#include "client.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println();
  Serial.println("Starting setup...");
  core::setup();
  core::api::setup();
  client::setup();
  Serial.println("Setup complete");
}

void loop()
{
  core::loop();
  core::api::loop();
  client::loop();
  // Uncomment the following line to print the delta time
  // Serial.print("Delta Time: ");
  // Serial.println(core::getDeltaTime());
}
