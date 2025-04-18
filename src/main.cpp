#include <Arduino.h>

#ifdef TRANSMITTER
  #include "transmitter.h"
#else
  #include "receiver.h"
#endif

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
}
