# WordClock NeoMatrix8x8 - Arduino CLI Setup

This document explains how to build and upload the WordClock project using Arduino CLI.

## Prerequisites

- Arduino CLI installed (✓ Done)
- Adafruit AVR board package installed (✓ Done)
- Arduino AVR core installed (✓ Done)

## Hardware Configuration

**Pro Trinket 5V (ATmega328P) Connections:**
- **RTC (DS1307):**
  - A2 → GND
  - A3 → PWR
  - A4 → SDA
  - A5 → SCL

- **NeoMatrix 8x8:**
  - Pin 6 → DIN
  - 5V → 5V
  - GND → GND

- **Adafruit AirLift WiFi:**
  - Pin 8 → Reset (ESP32_RESETN)
  - Pin 9 → Busy/ACK (SPIWIFI_ACK)
  - Pin 10 → Chip Select (SPIWIFI_SS)
  - Pin 11 → MOSI
  - Pin 12 → MISO
  - Pin 13 → SCK

## Building the Project

### Option 1: Using the build script
```bash
./build.sh
```

### Option 2: Manual compilation
```bash
arduino-cli compile --fqbn adafruit:avr:protrinket5 --libraries LIbraries WordClock_NeoMatrix8x8
```

## Uploading to Pro Trinket

### Step 1: Connect the Pro Trinket
1. Connect your Pro Trinket 5V to your computer via USB
2. Check available ports:
```bash
arduino-cli board list
```

### Step 2: Upload using the script
```bash
./upload.sh /dev/cu.usbmodem[YOUR_PORT_NUMBER]
```

### Step 3: Manual upload (alternative)
```bash
arduino-cli upload -p /dev/cu.usbserial-[YOUR_PORT_NUMBER] --fqbn adafruit:avr:protrinket5ftdi WordClock_NeoMatrix8x8
```

## Memory Usage

The compiled sketch uses approximately **28,628 bytes (99%)** of the Pro Trinket's 28,672 bytes of program storage space. This is very close to the limit but still fits.

## WiFi Configuration

The project is configured to connect to WiFi for NTP time synchronization:
- **SSID:** "Carti Vamps Only"
- **Password:** "metamorphosis"
- **Timezone:** GMT-5 (Eastern Time)

To change WiFi credentials, edit the `get_time.ino` file:
```cpp
char ssid[] = "YOUR_NETWORK_NAME";
char pass[] = "YOUR_PASSWORD";
```

## Troubleshooting

### Build Issues
- **Library not found:** Ensure all libraries are in the `LIbraries/` folder
- **Memory issues:** The sketch uses 99% of available space - any modifications may cause overflow

### Upload Issues
- **Port not found:** Make sure the Pro Trinket is connected and drivers are installed
- **Upload fails:** Try putting the Pro Trinket in bootloader mode by pressing reset while plugging in USB

### Runtime Issues
- **WiFi connection fails:** Check network credentials and signal strength
- **Time not updating:** Verify NTP server accessibility and timezone settings
- **Display issues:** Check NeoMatrix wiring and power supply

## Project Features

- **WiFi Time Sync:** Automatically syncs time via NTP at startup and daily at midnight
- **DST Support:** Automatically adjusts for Daylight Saving Time
- **Color Effects:** Rainbow color cycling and word flash animations
- **Brightness Control:** Automatic day/night brightness adjustment
- **Word Display:** Shows time in English words on 8x8 matrix

## Library Dependencies

All required libraries are included in the `LIbraries/` folder:
- RTClib (RTC functionality)
- DST_RTC (Daylight Saving Time calculations)
- Adafruit_GFX (Graphics library)
- Adafruit_NeoPixel (NeoPixel control)
- Adafruit_NeoMatrix (NeoMatrix support)
- WiFiNINA (WiFi connectivity)
- NTPClient (Network time protocol)
- Time (Time utilities)
- Adafruit_BusIO (I2C/SPI communication)
- TinyWireM (I2C for small microcontrollers)
