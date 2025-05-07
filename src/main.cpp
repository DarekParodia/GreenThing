#include <Arduino.h>

#include "core/core.h"
#include "client.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
  core::setup();
  client::setup();
}

void loop() {
  core::loop();
  client::loop();
  // Uncomment the following line to print the delta time
  // Serial.print("Delta Time: ");
  // Serial.println(core::getDeltaTime());
}
