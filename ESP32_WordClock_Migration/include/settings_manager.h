#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

// DST rule types
enum DSTRules {
  DST_DISABLED,
  DST_US,
  DST_EU
};

// Settings structure
struct ClockSettings {
  int timezoneOffset;     // Hours from UTC (-12 to +12)
  DSTRules dstRules;      // DST calculation rules
  int brightness;         // Display brightness (0-100)
  bool firstTimeSetup;    // Flag for initial setup
};

class SettingsManager {
private:
  Preferences preferences;
  ClockSettings currentSettings;
  
  // Default settings
  static const ClockSettings defaultSettings;
  
  // Timezone info structure
  struct TimezoneInfo {
    int offset;
    const char* name;
    const char* description;
  };
  
  // Common timezone definitions
  static const TimezoneInfo timezones[];
  static const int timezoneCount;

public:
  // Constructor
  SettingsManager();
  
  // Initialization
  bool initialize();
  void cleanup();
  
  // Settings management
  bool loadSettings();
  bool saveSettings();
  void resetToDefaults();
  
  // Getters
  ClockSettings getSettings() const;
  int getTimezoneOffset() const;
  DSTRules getDSTRules() const;
  int getBrightness() const;
  bool isFirstTimeSetup() const;
  
  // Setters
  void setTimezoneOffset(int offset);
  void setDSTRules(DSTRules rules);
  void setBrightness(int brightness);
  void setFirstTimeSetup(bool firstTime);
  
  // Timezone helpers
  String getTimezoneDisplayName(int offset) const;
  String getCurrentTimezoneDisplayName() const;
  int getNextTimezoneOffset(int currentOffset) const;
  int getPreviousTimezoneOffset(int currentOffset) const;
  
  // DST helpers
  String getDSTRulesDisplayName(DSTRules rules) const;
  DSTRules getNextDSTRules(DSTRules currentRules) const;
  
  // Validation
  bool isValidTimezoneOffset(int offset) const;
  bool isValidBrightness(int brightness) const;
  
  // Status
  String getStatusString() const;
};

// Global settings manager instance
extern SettingsManager settingsManager;

#endif // SETTINGS_MANAGER_H
