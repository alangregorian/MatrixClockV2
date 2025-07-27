# WordClock Version History

## Version 0.1.4 (Critical Stability Fixes)
**Date**: January 26, 2025
**Critical Bug Fixes**:
- **MAJOR**: Fixed recurring crash issues that occurred whenever new code was added to the project
- **MAJOR**: Resolved memory corruption in display functions causing system instability
- **MAJOR**: Fixed unsafe String operations causing heap fragmentation and crashes
- Fixed crash in `displayCurrentNetwork()` function after WiFi network scanning
- Fixed crash in `displayPasswordEntry()` function during password input
- Eliminated external variable dependencies causing undefined behavior

**Memory Management Improvements**:
- Replaced unsafe String concatenations with fixed-size char arrays
- Added bounds checking for all string operations in display functions
- Implemented safe string truncation with proper null termination
- Added ESP32-S2 specific memory optimization compiler flags
- Enabled PSRAM support and huge_app partition scheme for better memory management
- Limited WiFi network scanning to top 10 networks to prevent memory overflow

**Display System Stability**:
- Refactored `displayCurrentNetwork()` to use passed parameters instead of external variables
- Refactored `displayPasswordEntry()` with safe string handling and fixed buffers
- Temporarily disabled NeoMatrix operations to improve stability
- Added proper include dependencies in state machine
- Fixed function signatures to match updated display manager interface

**Build System Enhancements**:
- Added memory optimization flags: `-fstack-protector`, `-fno-exceptions`
- Enabled ESP32-S2 PSRAM support with `CONFIG_SPIRAM_SUPPORT=y`
- Added huge app partition scheme for larger program space
- Improved compiler flags for better memory management

**Technical Improvements**:
- Enhanced parameter passing between modules to eliminate unsafe global access
- Improved code robustness with defensive programming practices
- Better separation of concerns between display and data management
- More predictable memory usage patterns

**Impact**: These fixes resolved the fundamental stability issues that made the system crash whenever any modifications were made to the codebase. The system is now stable and can handle feature additions without memory-related crashes.

## Version 0.1.3 (WiFi Password Entry System)
**Date**: January 26, 2025
**Features Added**:
- Replaced 'prev' with 'select' functionality in WiFi results
- Added complete character-by-character password entry system
- Implemented STATE_PASSWORD_ENTRY with full character set support
- Added automatic open network detection (skips password entry)
- Created displayPasswordEntry() function with visual feedback
- Added WiFi connection functionality with timeout handling
- Updated button mappings: A=Next, B=Select, C=Rescan
- Password entry: A=Cycle Char, B=Confirm Char, C=Submit
- Added password masking and current character display
- Improved user experience with proper state transitions

**Technical Improvements**:
- Enhanced state machine with password entry state
- Comprehensive character set for password input
- Automatic network type detection and handling
- Improved WiFi connection workflow

## Version 0.0.3 (Display Manager Refactoring)
**Date**: January 25, 2025
**Features Added**:
- Added dedicated display clearing functions for better readability
- Implemented clearTFTScreen() with optional color parameter
- Added clearNeoMatrix() function for LED matrix clearing
- Created clearAllDisplays() convenience function
- Refactored all display functions to use new clear functions
- Improved code maintainability and consistency

**Technical Improvements**:
- Better separation of display clearing logic
- More consistent display management across all functions
- Enhanced code organization and readability

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
- **0.2.0**: NTP time synchronization
- **0.3.0**: WordClock display mode
- **0.4.0**: Settings and configuration management
- **1.0.0**: Full feature release
