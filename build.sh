#!/bin/bash
# This script builds and uploads the project using platformio
# configure platformio.ini for the target board and serial ports

# pio run -t clean

# build the project
# pio run --environment esp8266_tx

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

pio run --environment esp8266_rx
# check if the build was successful
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi


# upload the project
# pio run --target upload --environment esp8266_tx
pio run --target upload --environment esp8266_rx

# enter new tmux instance and monitor pio on tmux split screen, exit after user presses enter
# tmux new-session -d -s pio_monitor
# tmux split-window -h
# tmux send-keys -t pio_monitor:0.0 'pio device monitor --environment esp8266_tx && exit' C-m
# tmux send-keys -t pio_monitor:0.1 'pio device monitor --environment esp8266_rx && exit' C-m
pio device monitor --environment esp8266_rx
# tmux attach-session -t pio_monitor
# wait for user to press enter
# read -p "Press enter to exit..."

