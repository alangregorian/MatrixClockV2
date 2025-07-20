#!/bin/bash

# ESP32-S2 WordClock Migration - Build Script
# Full build process including dependency checks

echo "ESP32-S2 WordClock Migration - Build Process"
echo "============================================"

# Check if Arduino CLI is installed
if ! command -v arduino-cli &> /dev/null; then
    echo "❌ Arduino CLI not found!"
    echo "Please install Arduino CLI: https://arduino.github.io/arduino-cli/"
    exit 1
fi

echo "✅ Arduino CLI found"

# Check if ESP32 core is installed
echo "Checking ESP32 core installation..."
if ! arduino-cli core list | grep -q "esp32:esp32"; then
    echo "❌ ESP32 core not installed!"
    echo "Installing ESP32 core..."
    arduino-cli core update-index
    arduino-cli core install esp32:esp32
    if [ $? -ne 0 ]; then
        echo "❌ Failed to install ESP32 core"
        exit 1
    fi
else
    echo "✅ ESP32 core found"
fi

# Check if required libraries are available
echo "Checking required libraries..."
REQUIRED_LIBS=("Adafruit GFX Library" "Adafruit ST7735 and ST7789 Library" "Adafruit NeoPixel" "Adafruit NeoMatrix")

for lib in "${REQUIRED_LIBS[@]}"; do
    if ! arduino-cli lib list | grep -q "$lib"; then
        echo "⚠️  Library '$lib' not found, attempting to install..."
        arduino-cli lib install "$lib"
        if [ $? -ne 0 ]; then
            echo "❌ Failed to install library: $lib"
            echo "You may need to install it manually"
        else
            echo "✅ Installed library: $lib"
        fi
    else
        echo "✅ Library found: $lib"
    fi
done

# Check project structure
echo "Checking project structure..."
if [ ! -f "main.cpp" ]; then
    echo "❌ main.cpp not found!"
    exit 1
fi

if [ ! -d "src" ] || [ ! -d "include" ]; then
    echo "❌ src/ or include/ directory not found!"
    exit 1
fi

echo "✅ Project structure valid"

# Run compilation
echo ""
echo "Starting compilation..."
./compile.sh

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 Build completed successfully!"
    echo ""
    echo "Next steps:"
    echo "1. Connect your ESP32-S2 Reverse TFT Feather"
    echo "2. Run: ./upload.sh <port>"
    echo "   Example: ./upload.sh /dev/cu.usbmodem101"
    echo ""
    echo "To find your port, run: arduino-cli board list"
else
    echo "❌ Build failed!"
    exit 1
fi
