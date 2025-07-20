#!/bin/bash

# WordClock NeoMatrix8x8 Upload Script
# This script uploads the compiled Arduino project to the Pro Trinket

# Check if a port is specified as argument
if [ $# -eq 0 ]; then
    echo "Usage: $0 <port>"
    echo "Example: $0 /dev/cu.usbmodem14101"
    echo ""
    echo "Available ports:"
    arduino-cli board list
    exit 1
fi

PORT=$1

echo "Uploading WordClock NeoMatrix8x8 to Pro Trinket on port $PORT..."

# First build the project
echo "Building project..."
arduino-cli compile --fqbn adafruit:avr:protrinket5 --libraries LIbraries WordClock_NeoMatrix8x8

if [ $? -ne 0 ]; then
    echo "Build failed! Cannot upload."
    exit 1
fi

# Upload to the board
echo "Uploading..."
arduino-cli upload -p $PORT --fqbn adafruit:avr:protrinket5ftdi WordClock_NeoMatrix8x8

if [ $? -eq 0 ]; then
    echo "Upload successful!"
else
    echo "Upload failed!"
    echo "Make sure:"
    echo "1. The Pro Trinket is connected via USB"
    echo "2. The correct port is specified"
    echo "3. The Pro Trinket is in bootloader mode (if needed)"
    exit 1
fi
