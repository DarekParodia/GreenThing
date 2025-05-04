#include <Arduino.h>
#include "core/core.h"

#ifdef TRANSMITTER
  #include "transmitter.h"
#else
  #include "receiver.h"
#endif

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
  core::setup();
  #ifdef TRANSMITTER
    transmitter::setup();
  #else
    receiver::setup();
  #endif
}

void loop() {
  core::loop();
  #ifdef TRANSMITTER
    transmitter::loop();
  #else
    receiver::loop();
  #endif
  // Uncomment the following line to print the delta time
  // Serial.print("Delta Time: ");
  // Serial.println(core::getDeltaTime());
}
