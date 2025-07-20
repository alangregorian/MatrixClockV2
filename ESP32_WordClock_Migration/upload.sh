#!/bin/bash

# ESP32-S2 WordClock Migration - Upload Script
# Uploads the compiled project to ESP32-S2 Reverse TFT Feather

BOARD_FQBN="esp32:esp32:adafruit_feather_esp32s2_reversetft"

# Function to auto-detect ESP32-S2 board
detect_esp32s2_port() {
    echo "🔍 Auto-detecting ESP32-S2 board..." >&2
    
    # Get list of connected boards and filter for ESP32-S2
    local boards_output=$(arduino-cli board list 2>/dev/null)
    
    # Look for ESP32-S2 in the board list (case insensitive)
    local esp32s2_ports=$(echo "$boards_output" | grep -i "esp32.*s2\|esp32s2\|adafruit.*feather.*esp32.*s2" | awk '{print $1}')
    
    # If no ESP32-S2 found by name, look for any ESP32 boards
    if [ -z "$esp32s2_ports" ]; then
        esp32s2_ports=$(echo "$boards_output" | grep -i "esp32" | awk '{print $1}')
    fi
    
    # Count how many ports we found
    local port_count=$(echo "$esp32s2_ports" | grep -v "^$" | wc -l)
    
    if [ $port_count -eq 0 ]; then
        echo "❌ No ESP32 boards detected!" >&2
        echo "" >&2
        echo "Available ports:" >&2
        arduino-cli board list >&2
        echo "" >&2
        echo "Please ensure your ESP32-S2 is connected via USB." >&2
        echo "You can also manually specify a port: $0 <port>" >&2
        return 1
    elif [ $port_count -eq 1 ]; then
        local detected_port=$(echo "$esp32s2_ports" | grep -v "^$")
        echo "✅ Found ESP32 board on port: $detected_port" >&2
        echo "$detected_port"
        return 0
    else
        echo "⚠️  Multiple ESP32 boards detected:" >&2
        echo "$boards_output" | grep -i "esp32" >&2
        echo "" >&2
        echo "Please specify which port to use:" >&2
        local i=1
        while IFS= read -r port; do
            if [ -n "$port" ]; then
                echo "  $i) $port" >&2
                i=$((i + 1))
            fi
        done <<< "$esp32s2_ports"
        
        echo "" >&2
        read -p "Select port number (1-$((i-1))) or press Enter to use first port: " selection
        
        if [ -z "$selection" ]; then
            selection=1
        fi
        
        local selected_port=$(echo "$esp32s2_ports" | sed -n "${selection}p")
        if [ -n "$selected_port" ]; then
            echo "Selected port: $selected_port" >&2
            echo "$selected_port"
            return 0
        else
            echo "❌ Invalid selection!" >&2
            return 1
        fi
    fi
}

# Determine which port to use
if [ $# -eq 0 ]; then
    # No port specified, try to auto-detect
    PORT=$(detect_esp32s2_port)
    if [ $? -ne 0 ]; then
        echo ""
        echo "Auto-detection failed. You can manually specify a port:"
        echo "Usage: $0 <port>"
        echo "Example: $0 /dev/cu.usbmodem101"
        exit 1
    fi
else
    # Port specified as argument
    PORT=$1
    echo "Using manually specified port: $PORT"
fi

echo "ESP32-S2 WordClock Migration - Upload"
echo "====================================="
echo "Port: $PORT"
echo "Board: $BOARD_FQBN"
echo ""

# Check if build artifacts exist, rebuild if necessary
if [ ! -d "build" ]; then
    echo "⚠️  Build artifacts not found. Building project first..."
    echo ""
    
    # Run the build script
    if [ -f "./build.sh" ]; then
        ./build.sh
        if [ $? -ne 0 ]; then
            echo "❌ Build failed! Cannot proceed with upload."
            exit 1
        fi
        echo ""
        echo "✅ Build completed. Proceeding with upload..."
    else
        echo "❌ Build script not found!"
        echo "Please ensure you're in the project directory and build.sh exists."
        exit 1
    fi
fi

# Create temporary sketch for upload
TEMP_SKETCH_DIR="temp_sketch"
SKETCH_NAME="ESP32_WordClock_Migration"

echo "Preparing upload..."

# Clean up any existing temp directory
rm -rf "$TEMP_SKETCH_DIR"

# Create temporary sketch structure
mkdir -p "$TEMP_SKETCH_DIR/$SKETCH_NAME"

# Copy main.cpp as .ino file
cp main.cpp "$TEMP_SKETCH_DIR/$SKETCH_NAME/$SKETCH_NAME.ino"

# Copy source files
cp -r src "$TEMP_SKETCH_DIR/$SKETCH_NAME/"
cp -r include "$TEMP_SKETCH_DIR/$SKETCH_NAME/"

# Copy build artifacts if they exist
if [ -d "build" ]; then
    cp -r build "$TEMP_SKETCH_DIR/$SKETCH_NAME/"
fi

echo "Uploading to ESP32-S2..."

# Upload to the board
arduino-cli upload -p "$PORT" --fqbn "$BOARD_FQBN" "$TEMP_SKETCH_DIR/$SKETCH_NAME"

# Check upload result
if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 Upload successful!"
    echo ""
    echo "Your ESP32-S2 should now be running the WordClock Migration firmware."
    echo ""
    echo "To monitor serial output, run:"
    echo "arduino-cli monitor -p $PORT -c baudrate=115200"
    echo ""
    echo "Press CTRL-C to exit the monitor."
else
    echo ""
    echo "❌ Upload failed!"
    echo ""
    echo "Troubleshooting:"
    echo "1. Make sure the ESP32-S2 is connected via USB"
    echo "2. Verify the correct port is specified"
    echo "3. Try putting the ESP32-S2 in bootloader mode:"
    echo "   - Hold BOOT button, press and release RESET, then release BOOT"
    echo "4. Check that no other programs are using the serial port"
    
    # Clean up and exit
    rm -rf "$TEMP_SKETCH_DIR"
    exit 1
fi

# Clean up temporary directory
echo "Cleaning up..."
rm -rf "$TEMP_SKETCH_DIR"

echo "Upload complete!"
