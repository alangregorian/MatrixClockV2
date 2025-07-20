#!/bin/bash

# ESP32-S2 WordClock Migration - Compile Script
# Compiles the modular C++ project using Arduino CLI

echo "Compiling ESP32-S2 WordClock Migration..."

# Set the board FQBN for ESP32-S2 Reverse TFT Feather
BOARD_FQBN="esp32:esp32:adafruit_feather_esp32s2_reversetft"

# Create a temporary sketch directory for Arduino CLI
TEMP_SKETCH_DIR="temp_sketch"
SKETCH_NAME="ESP32_WordClock_Migration"

# Clean up any existing temp directory
rm -rf "$TEMP_SKETCH_DIR"

# Create temporary sketch structure
mkdir -p "$TEMP_SKETCH_DIR/$SKETCH_NAME"

# Copy main.cpp as .ino file (Arduino CLI expects .ino)
cp main.cpp "$TEMP_SKETCH_DIR/$SKETCH_NAME/$SKETCH_NAME.ino"

# Copy source files as .cpp files
cp -r src "$TEMP_SKETCH_DIR/$SKETCH_NAME/"
cp -r include "$TEMP_SKETCH_DIR/$SKETCH_NAME/"

echo "Temporary sketch created in $TEMP_SKETCH_DIR"

# Compile the project
echo "Compiling with Arduino CLI..."
arduino-cli compile --fqbn "$BOARD_FQBN" "$TEMP_SKETCH_DIR/$SKETCH_NAME"

# Check compilation result
if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    
    # Show memory usage
    echo ""
    echo "Memory Usage:"
    arduino-cli compile --fqbn "$BOARD_FQBN" "$TEMP_SKETCH_DIR/$SKETCH_NAME" 2>&1 | grep -E "(Sketch uses|Global variables use)"
    
    # Copy compiled files back to main directory
    if [ -d "$TEMP_SKETCH_DIR/$SKETCH_NAME/build" ]; then
        echo "Copying build artifacts..."
        cp -r "$TEMP_SKETCH_DIR/$SKETCH_NAME/build" ./
    fi
    
else
    echo "❌ Compilation failed!"
    echo "Check the error messages above for details."
    exit 1
fi

# Clean up temporary directory
echo "Cleaning up temporary files..."
rm -rf "$TEMP_SKETCH_DIR"

echo "Compilation complete!"
