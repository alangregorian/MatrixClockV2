# ESP32-S2 WordClock Migration

This project migrates the original Arduino WordClock to a modern ESP32-S2 platform with proper C++ structure and state machine architecture.

## Hardware

- **Adafruit ESP32-S2 Reverse TFT Feather**
  - Built-in 240x135 ST7789 TFT display
  - Built-in WiFi
  - 3 tactile buttons (D0, D1, D2)
- **8x8 NeoMatrix** connected to pin 6 (optional)

## Project Structure

```
ESP32_WordClock_Migration/
├── main.cpp                    # Primary state machine and Arduino setup/loop
├── src/                        # Source files
│   ├── wifi_manager.cpp        # WiFi scanning and connection management
│   ├── display_manager.cpp     # TFT and NeoMatrix display handling
│   ├── button_handler.cpp      # Button input management
│   ├── state_machine.cpp       # State machine implementation
│   └── version.cpp              # Version information and changelog
├── include/                    # Header files
│   ├── wifi_manager.h
│   ├── display_manager.h
│   ├── button_handler.h
│   ├── state_machine.h
│   └── version.h                # Version tracking and build info
├── compile.sh                  # Compile script
├── build.sh                    # Full build process with dependency checks
├── upload.sh                   # Upload to ESP32-S2
├── CHANGELOG.md                # Version history and feature tracking
└── README.md                   # This file
```

## Features

### Current Implementation
- **Startup Logo Display**: Shows WordClock logo with version number for 4 seconds
- **Version Tracking**: Displays current version (0.0.1) and build date
- **WiFi Scanner**: Scans and displays available WiFi networks
- **TFT Display**: Shows network information with signal strength
- **NeoMatrix Visualization**: Signal strength indicator
- **Button Navigation**: 
  - D0: Next network
  - D1: Previous network  
  - D2: Rescan networks
- **State Machine**: Organized program flow with logo display state
- **Modular Architecture**: Clean separation of concerns with proper C++ structure

### Future Expansion (Ready for Implementation)
- WiFi connection to selected network
- NTP time synchronization
- WordClock display mode
- Configuration persistence

## Build System

This project uses **Arduino CLI** instead of Arduino IDE for compilation, providing:
- Proper C++ structure (no .ino file concatenation)
- Modular design with separate source files
- Cross-platform build scripts
- Dependency management

## Getting Started

### Prerequisites

1. **Arduino CLI** installed
2. **ESP32 core** for Arduino CLI
3. Required libraries:
   - Adafruit GFX Library
   - Adafruit ST7735 and ST7789 Library
   - Adafruit NeoPixel
   - Adafruit NeoMatrix

### Building and Uploading

1. **Full build with dependency checks:**
   ```bash
   ./build.sh
   ```

2. **Compile only:**
   ```bash
   ./compile.sh
   ```

3. **Upload to device:**
   ```bash
   ./upload.sh
   ```
   (Auto-detects ESP32-S2 board, or manually specify: `./upload.sh /dev/cu.usbmodem101`)

4. **Monitor serial output:**
   ```bash
   arduino-cli monitor -p /dev/cu.usbmodem101 -c baudrate=115200
   ```

### Finding Your Port

```bash
arduino-cli board list
```

## State Machine

The project uses a clean state machine architecture:

- `STATE_INIT`: Hardware initialization
- `STATE_LOGO_DISPLAY`: Shows startup logo with version for 4 seconds
- `STATE_WIFI_SCAN`: WiFi network scanning
- `STATE_WIFI_DISPLAY`: Display networks and handle navigation
- `STATE_WIFI_CONNECT`: Connect to selected network (future)
- `STATE_TIME_SYNC`: NTP time synchronization (future)
- `STATE_CLOCK_DISPLAY`: WordClock display mode (future)

### Startup Sequence
1. **Hardware Init** → **Logo Display** (4 seconds) → **WiFi Scan** → **WiFi Display**
2. The logo shows the WordClock name, version number, and build date
3. After 4 seconds, automatically transitions to WiFi scanning

## Pin Configuration

### ESP32-S2 Reverse TFT Feather
- **TFT Display:**
  - CS: GPIO42
  - RST: GPIO41
  - DC: GPIO40
  - MOSI: GPIO35
  - SCLK: GPIO36
  - Backlight: GPIO45

- **Buttons:**
  - D0: GPIO0 (BOOT button, pulled HIGH)
  - D1: GPIO1 (pulled LOW by hardware)
  - D2: GPIO2 (pulled LOW by hardware)

- **NeoMatrix:**
  - DIN: GPIO6

## Migration Benefits

### From Original .ino Structure
- ✅ **Proper C++ modules** instead of Arduino IDE file concatenation
- ✅ **State machine architecture** for clear program flow
- ✅ **Modular design** for easier maintenance and testing
- ✅ **Cross-platform build system** (works without Arduino IDE)
- ✅ **Scalable architecture** ready for WordClock features
- ✅ **Successful compilation** with Arduino CLI (910KB program, 60KB RAM)

### Hardware Upgrade
- ✅ **ESP32-S2** instead of Pro Trinket (more memory, WiFi, faster)
- ✅ **Built-in TFT display** for rich user interface
- ✅ **WiFi connectivity** for NTP time sync
- ✅ **More GPIO pins** for future expansion

## Migration Status

✅ **COMPLETE**: The ESP32-S2 WordClock Migration is now fully functional!

- ✅ **Compilation**: Successfully compiles with Arduino CLI
- ✅ **Upload**: Successfully uploads to ESP32-S2 hardware
- ✅ **Auto-rebuild**: Upload script automatically rebuilds if needed
- ✅ **Port detection**: Automatic ESP32-S2 board detection
- ✅ **Memory usage**: Efficient use of flash (63%) and RAM (18%)

## Development

### Adding New Features

1. **Create new module:**
   - Add header file in `include/`
   - Add implementation in `src/`
   - Include in `main.cpp`

2. **Add new state:**
   - Add to `SystemState` enum in `main.cpp`
   - Implement in main `switch` statement

3. **Test:**
   ```bash
   ./build.sh && ./upload.sh <port>
   ```

### Code Style
- Use proper C++ headers and implementations
- Follow existing naming conventions
- Document functions and complex logic
- Keep state machine transitions clear

## Troubleshooting

### Build Issues
- Ensure Arduino CLI is installed and in PATH
- Check ESP32 core installation: `arduino-cli core list`
- Verify required libraries: `arduino-cli lib list`

### Upload Issues
- Check USB connection and port
- Try bootloader mode: Hold BOOT, press RESET, release BOOT
- Verify correct board FQBN: `esp32:esp32:adafruit_feather_esp32s2_reversetft`

### Runtime Issues
- Monitor serial output for debug information
- Check button wiring and pull-up/pull-down configuration
- Verify TFT display initialization

## License

Based on the original WordClock project. See LICENSE file for details.
