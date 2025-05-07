#!/bin/bash
# This script builds and uploads the project using platformio
# configure platformio.ini for the target board and serial ports

# pio run -t clean

# upload the project
pio run --target upload --environment esp8266

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

pio device monitor --environment esp8266
