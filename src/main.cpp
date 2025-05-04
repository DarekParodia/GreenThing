#include <Arduino.h>
#include "core/core.h"

#ifdef TRANSMITTER
  #include "transmitter.h"
#else
  #include "receiver.h"
#endif

void calculateDeltaTime() {
  core::currentLoopTime = millis();
  core::deltaTime = core::currentLoopTime - core::lastLoopTime;

  // Ensure the loop time is consistent
  if (core::deltaTime < core::loopTime) {
    delay(core::loopTime - core::deltaTime);
  }
  core::lastLoopTime = millis();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) ;

  #ifdef TRANSMITTER
    transmitter::setup();
  #else
    receiver::setup();
  #endif
}

void loop() {
  #ifdef TRANSMITTER
    transmitter::loop();
  #else
    receiver::loop();
  #endif

  calculateDeltaTime();
  // Uncomment the following line to print the delta time
  Serial.print("Delta Time: ");
  Serial.println(core::deltaTime);
}
