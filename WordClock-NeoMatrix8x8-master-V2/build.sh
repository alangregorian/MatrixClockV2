#!/bin/bash

# WordClock NeoMatrix8x8 Build Script
# This script compiles the Arduino project using Arduino CLI

echo "Building WordClock NeoMatrix8x8..."

# Compile the project
arduino-cli compile --fqbn adafruit:avr:protrinket5 --libraries LIbraries WordClock_NeoMatrix8x8

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Sketch uses $(arduino-cli compile --fqbn adafruit:avr:protrinket5 --libraries LIbraries WordClock_NeoMatrix8x8 2>&1 | grep 'Sketch uses' | cut -d' ' -f3) bytes of program storage space."
else
    echo "Build failed!"
    exit 1
fi
