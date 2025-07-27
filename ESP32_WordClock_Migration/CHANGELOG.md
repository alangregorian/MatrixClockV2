# WordClock Version History

## Version 0.4.1 (Critical Time Sync Bug Fixes)
**Date**: July 27, 2025
**Critical Bug Fixes**:
- **MAJOR**: Fixed critical time calculation bug in `getCurrentEpoch()` causing incorrect date/time display
- **MAJOR**: Resolved time drift issue where clock showed wrong dates (e.g., August 31st instead of July 27th)
- **MAJOR**: Fixed `needsTimeSync()` function using incorrect time comparison logic
- Fixed timezone offset application - now properly stores UTC time and applies offset during display
- Improved DST calculation with proper Sunday calculation algorithm for US Central Time
- Added proper tracking of sync timing with `lastSyncMillis` variable

**Time Calculation Improvements**:
- Separated UTC time storage from timezone-adjusted display time
- Fixed millis() overflow handling in time calculations
- Corrected elapsed time calculation since last NTP sync
- Improved time precision and eliminated accumulating errors
- Added comprehensive debug logging for time sync operations

**DST Algorithm Enhancements**:
- Replaced simplified DST logic with accurate day-of-week calculations
- Proper second Sunday in March and first Sunday in November calculations
- Fixed DST transition dates for US Central Time zone
- Eliminated hardcoded date assumptions that caused incorrect DST application

**Technical Improvements**:
- Added `lastSyncMillis` tracking variable to header and implementation
- Enhanced debug output showing UTC epoch, sync timing, and timezone offsets
- Better separation of UTC time storage and local time display
- Improved error handling and validation in time calculations
- More robust time sync interval checking

**Impact**: These fixes resolve the fundamental time synchronization issues that caused the clock to display incorrect dates and times, ensuring accurate timekeeping aligned with actual current time.

## Version 0.4.0 (Time Management & NTP Synchronization)
**Date**: January 27, 2025
**Features Added**:
- **NEW**: Complete time management system with NTP synchronization
- **NEW**: Added comprehensive `TimeManager` class for time operations
- **NEW**: Implemented NTP client functionality for automatic time sync
- **NEW**: Added time sync status display with visual feedback
- **NEW**: Created clock display screen showing current time and date
- Added new `STATE_TIME_SYNC` and `STATE_CLOCK_DISPLAY` states to state machine
- Implemented automatic DST (Daylight Saving Time) calculation for US Central Time
- Added periodic time synchronization with configurable intervals

**Time Manager Features**:
- NTP synchronization with pool.ntp.org servers
- Automatic timezone offset calculation with DST support
- Multiple time format functions: `getFormattedTime()`, `getFormattedDate()`, `getFormattedDateTime()`
- Individual time component access: `getHours()`, `getMinutes()`, `getSeconds()`, etc.
- Time sync status tracking with enum states (NOT_SYNCED, SYNCING, SUCCESS, FAILED)
- Configurable sync intervals and force sync capability
- Time validation and status reporting

**Display Functions Added**:
- `displayTimeSyncStatus()`: Shows NTP sync progress with color-coded status
- `displayCurrentTime()`: Large time display with date and timezone info
- `displayClockScreen()`: Complete clock interface with time, date, status, and controls
- Color-coded sync status (Yellow=Syncing, Green=Synced, Red=Failed)
- Real-time clock updates every second

**State Machine Enhancements**:
- Enhanced state flow: WiFi Success → Time Sync → Clock Display
- Time sync state with automatic progression and error handling
- Clock display state with button controls (A=Settings, B=Force Sync, C=WiFi)
- Background time sync checks and automatic re-sync when needed
- Proper error handling and fallback behavior

**Technical Improvements**:
- Added WiFiUDP and NTPClient integration for time synchronization
- Implemented DST calculation algorithm for US Central Time zone
- Added time manager initialization and cleanup functions
- Enhanced state machine with time-related state handlers
- Memory-efficient time management with minimal heap usage
- Robust error handling for network time sync failures

**User Experience Enhancements**:
- Clear visual feedback during time synchronization process
- Large, easy-to-read time display with date information
- Intuitive button controls for time sync and navigation
- Automatic time updates without user intervention
- Graceful handling of sync failures with retry options
- Timezone information display for user awareness

**Clock Display Features**:
- Large 3x size time display (HH:MM:SS format)
- Date display in YYYY-MM-DD format
- Sync status indicator with color coding
- Central Time (US) timezone label
- Button instructions: A=Settings, B=Sync, C=WiFi
- Real-time updates every second
- Force sync capability via button B

## Version 0.3.4 (WiFi Success & Failure Screens)
**Date**: January 26, 2025
**Features Added**:
- **NEW**: Added WiFi connection success screen with detailed connection information
- **NEW**: Added WiFi connection failure screen with retry options
- Added new `STATE_WIFI_SUCCESS` and `STATE_WIFI_FAILURE` states to state machine
- Implemented `displayWiFiSuccess()` function showing IP address, signal strength, and connection details
- Implemented `displayWiFiFailure()` function with multiple recovery options
- Enhanced user experience with clear success/failure feedback and actionable options

**Success Screen Features**:
- "WiFi Connected!" confirmation message with green styling
- Network name display with safe string truncation
- IP address obtained from connection
- Signal strength display with visual signal bars
- "Connection successful!" status message
- "Press any button to continue" instruction for progression

**Failure Screen Features**:
- "Connection Failed" error message with red styling
- Network name that failed to connect
- Possible failure causes listed (wrong password, weak signal, network unavailable)
- Three recovery options: "A: Retry", "B: New Pass", "C: Back"
- Clear visual feedback with appropriate color coding

**Enhanced State Flow**:
- `STATE_WIFI_CONNECTING` → (success) → `STATE_WIFI_SUCCESS` → (button press) → `STATE_TIME_SYNC`
- `STATE_WIFI_CONNECTING` → (failure) → `STATE_WIFI_FAILURE` → (user choice) → appropriate recovery state
- Success screen: Any button press continues to time synchronization
- Failure screen: A=Retry same password, B=Enter new password, C=Back to network selection

**Technical Improvements**:
- Added success and failure state handlers to state machine
- Enhanced display manager with comprehensive success/failure display functions
- Improved error recovery workflow with multiple user options
- Better separation of connection results from connection logic
- Safe string handling in all new display functions to prevent memory issues

**User Experience Enhancements**:
- Clear visual confirmation of successful connections with connection details
- Comprehensive failure handling with multiple recovery paths
- No more ambiguity about connection status
- Users can easily retry, change passwords, or select different networks
- Consistent visual design with appropriate color coding for success/failure states

## Version 0.2.4 (WiFi Connection UI Enhancement)
**Date**: January 26, 2025
**Features Added**:
- **NEW**: Added "Connecting..." display message on TFT when password is submitted
- Added new `STATE_WIFI_CONNECTING` state for better user feedback
- Implemented `displayConnectingMessage()` function with network name display
- Enhanced user experience with immediate visual feedback during connection attempts
- Added connection timeout display (10 seconds) for user awareness
- Improved state flow: Password Entry → Connecting Display → Connection Result

**Technical Improvements**:
- Added new connecting state to state machine architecture
- Enhanced display manager with connecting message function
- Improved state transitions for both secured and open networks
- Better separation of connection logic from display logic
- Consistent "Connecting..." experience for all network types

**User Experience Enhancements**:
- Users now see immediate feedback when submitting passwords
- Clear indication that system is actively attempting connection
- Network name displayed during connection attempt
- Timeout information provided to set user expectations
- No more "frozen" appearance during connection attempts

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
- **0.5.0**: WordClock display mode with LED matrix word illumination
- **0.6.0**: Settings and configuration management
- **0.7.0**: Advanced features (brightness control, color themes, etc.)
- **1.0.0**: Full feature release with complete WordClock functionality
