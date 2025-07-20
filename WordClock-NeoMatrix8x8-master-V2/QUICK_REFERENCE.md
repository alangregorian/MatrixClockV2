# WordClock Arduino CLI - Quick Reference

## Essential Commands

### Build the project
```bash
./build.sh
```

### Check available ports
```bash
arduino-cli board list
```

### Upload to Pro Trinket
```bash
./upload.sh /dev/cu.usbserial-[PORT_NUMBER]
```

## Manual Commands (if scripts don't work)

### Compile
```bash
arduino-cli compile --fqbn adafruit:avr:protrinket5 --libraries LIbraries WordClock_NeoMatrix8x8
```

### Upload
```bash
arduino-cli upload -p /dev/cu.usbserial-[PORT] --fqbn adafruit:avr:protrinket5ftdi WordClock_NeoMatrix8x8
```

## Status Check

### Verify Arduino CLI setup
```bash
arduino-cli version
arduino-cli core list
arduino-cli board listall | grep trinket
```

### Check library configuration
```bash
arduino-cli config dump
```

## Memory Usage
- **Program Storage:** 28,080 / 28,672 bytes (97% used)
- **Dynamic Memory:** 1,152 bytes used

✅ **Improved:** Memory usage reduced from 99% to 97% after DST optimization.

## WiFi Settings
- **Network:** "AJG 5GHz"
- **Password:** "letmeinplease"
- **Timezone:** GMT-6 (Chicago Central Time)

Edit `WordClock_NeoMatrix8x8/get_time.ino` to change WiFi credentials.
