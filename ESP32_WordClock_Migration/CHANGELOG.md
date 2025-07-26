# WordClock Version History

## Version 0.0.2 (Display & WiFi Improvements)
**Date**: January 25, 2025
**Features Added**:
- Fixed display clearing issue between state transitions
- Implemented complete WiFi networks display screen
- Added `displayCurrentNetwork()` function with full network information
- Visual signal strength bars with color coding
- Security type display with color coding (Red=Open, Yellow=WEP, Green=WPA/WPA2)
- Network navigation controls (A=Next, B=Previous, C=Rescan)
- Network counter display (e.g., "Network 1 of 5")
- Proper screen clearing in all display functions
- SSID truncation for long network names

**Bug Fixes**:
- Clock logo no longer persists when transitioning to WiFi screen
- All display functions now properly clear screen before drawing new content
- Fixed compilation error with undefined color constants

**Technical Improvements**:
- Enhanced display manager with comprehensive WiFi display function
- Better state transition handling with proper display updates
- Improved code organization and function declarations

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
- **0.0.3**: WiFi connection functionality
- **0.1.0**: NTP time synchronization
- **0.2.0**: WordClock display mode
- **0.3.0**: Settings and configuration management
- **1.0.0**: Full feature release
