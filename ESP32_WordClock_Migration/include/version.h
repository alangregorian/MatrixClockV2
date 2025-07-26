#ifndef VERSION_H
#define VERSION_H

#define SOFTWARE_VERSION "0.0.1"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

// Version history and changelog
const char* VERSION_CHANGELOG = 
  "Version 0.0.1 - Initial Release\n"
  "- Added startup logo display with version number\n"
  "- WiFi network scanning functionality\n"
  "- Basic state machine architecture\n"
  "- TFT display support (240x135 ST7789)\n"
  "- Button handling system\n"
  "- Modular code structure\n";

#endif // VERSION_H
