# WordClock Version History

## Version 0.0.1 (Initial Release)
**Date**: January 25, 2025
**Features Added**:
- Startup logo display with version number (4-second display)
- WiFi network scanning functionality
- State machine architecture with modular design
- TFT display initialization and management (240x135 ST7789)
- Button handling system (3 tactile buttons)
- Modular code structure with separate managers
- Version tracking system with build timestamps

**Hardware Support**:
- ESP32-S2 Reverse TFT Feather
- 240x135 ST7789 TFT display
- 8x8 NeoMatrix (optional, startup pattern disabled)
- 3 tactile buttons (D0, D1, D2)

**Known Issues**:
- NeoMatrix startup pattern disabled (causes reset)
- WiFi connection not yet implemented
- Time synchronization not yet implemented
- WordClock display mode not yet implemented

**Technical Details**:
- State machine with 6 defined states
- Modular architecture with separate header/source files
- Version management system with changelog tracking

## Future Planned Versions
- **0.0.2**: WiFi connection functionality
- **0.1.0**: NTP time synchronization
- **0.2.0**: WordClock display mode
- **0.3.0**: Settings and configuration management
- **1.0.0**: Full feature release
