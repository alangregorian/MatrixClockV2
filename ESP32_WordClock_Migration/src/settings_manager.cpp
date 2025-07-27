#include "../include/settings_manager.h"

// Global settings manager instance
SettingsManager settingsManager;

// Default settings
const ClockSettings SettingsManager::defaultSettings = {
  -6,           // timezoneOffset: UTC-6 (CST)
  DST_US,       // dstRules: US DST rules
  75,           // brightness: 75%
  true          // firstTimeSetup: true
};

// Common timezone definitions
const SettingsManager::TimezoneInfo SettingsManager::timezones[] = {
  {-12, "UTC-12", "Baker Island Time"},
  {-11, "UTC-11", "Hawaii-Aleutian Standard Time"},
  {-10, "UTC-10", "Hawaii Standard Time"},
  {-9,  "UTC-9",  "Alaska Standard Time"},
  {-8,  "UTC-8",  "Pacific Standard Time"},
  {-7,  "UTC-7",  "Mountain Standard Time"},
  {-6,  "UTC-6",  "Central Standard Time"},
  {-5,  "UTC-5",  "Eastern Standard Time"},
  {-4,  "UTC-4",  "Atlantic Standard Time"},
  {-3,  "UTC-3",  "Argentina Time"},
  {-2,  "UTC-2",  "South Georgia Time"},
  {-1,  "UTC-1",  "Azores Time"},
  {0,   "UTC+0",  "Greenwich Mean Time"},
  {1,   "UTC+1",  "Central European Time"},
  {2,   "UTC+2",  "Eastern European Time"},
  {3,   "UTC+3",  "Moscow Time"},
  {4,   "UTC+4",  "Gulf Standard Time"},
  {5,   "UTC+5",  "Pakistan Standard Time"},
  {6,   "UTC+6",  "Bangladesh Standard Time"},
  {7,   "UTC+7",  "Indochina Time"},
  {8,   "UTC+8",  "China Standard Time"},
  {9,   "UTC+9",  "Japan Standard Time"},
  {10,  "UTC+10", "Australian Eastern Standard Time"},
  {11,  "UTC+11", "Solomon Islands Time"},
  {12,  "UTC+12", "New Zealand Standard Time"}
};

const int SettingsManager::timezoneCount = sizeof(SettingsManager::timezones) / sizeof(SettingsManager::timezones[0]);

// Constructor
SettingsManager::SettingsManager() {
  currentSettings = defaultSettings;
}

// Initialize the settings manager
bool SettingsManager::initialize() {
  Serial.println("[SETTINGS_MANAGER] Initializing...");
  
  // Initialize preferences with namespace "clocksettings"
  if (!preferences.begin("clocksettings", false)) {
    Serial.println("[SETTINGS_MANAGER] Failed to initialize preferences");
    return false;
  }
  
  // Load existing settings or use defaults
  if (!loadSettings()) {
    Serial.println("[SETTINGS_MANAGER] No saved settings found, using defaults");
    currentSettings = defaultSettings;
    saveSettings(); // Save defaults for next time
  }
  
  Serial.printf("[SETTINGS_MANAGER] Initialized - Timezone: %d, DST: %d, Brightness: %d\n", 
                currentSettings.timezoneOffset, currentSettings.dstRules, currentSettings.brightness);
  return true;
}

// Cleanup resources
void SettingsManager::cleanup() {
  preferences.end();
}

// Load settings from flash memory
bool SettingsManager::loadSettings() {
  Serial.println("[SETTINGS_MANAGER] Loading settings from flash...");
  
  // Check if settings exist
  if (!preferences.isKey("timezone")) {
    Serial.println("[SETTINGS_MANAGER] No saved settings found");
    return false;
  }
  
  // Load each setting with validation
  int timezone = preferences.getInt("timezone", defaultSettings.timezoneOffset);
  int dstRules = preferences.getInt("dstRules", defaultSettings.dstRules);
  int brightness = preferences.getInt("brightness", defaultSettings.brightness);
  bool firstTime = preferences.getBool("firstTime", defaultSettings.firstTimeSetup);
  
  // Validate loaded values
  if (!isValidTimezoneOffset(timezone)) {
    Serial.printf("[SETTINGS_MANAGER] Invalid timezone %d, using default\n", timezone);
    timezone = defaultSettings.timezoneOffset;
  }
  
  if (dstRules < DST_DISABLED || dstRules > DST_EU) {
    Serial.printf("[SETTINGS_MANAGER] Invalid DST rules %d, using default\n", dstRules);
    dstRules = defaultSettings.dstRules;
  }
  
  if (!isValidBrightness(brightness)) {
    Serial.printf("[SETTINGS_MANAGER] Invalid brightness %d, using default\n", brightness);
    brightness = defaultSettings.brightness;
  }
  
  // Apply loaded settings
  currentSettings.timezoneOffset = timezone;
  currentSettings.dstRules = (DSTRules)dstRules;
  currentSettings.brightness = brightness;
  currentSettings.firstTimeSetup = firstTime;
  
  Serial.printf("[SETTINGS_MANAGER] Settings loaded successfully\n");
  return true;
}

// Save settings to flash memory
bool SettingsManager::saveSettings() {
  Serial.println("[SETTINGS_MANAGER] Saving settings to flash...");
  
  preferences.putInt("timezone", currentSettings.timezoneOffset);
  preferences.putInt("dstRules", (int)currentSettings.dstRules);
  preferences.putInt("brightness", currentSettings.brightness);
  preferences.putBool("firstTime", currentSettings.firstTimeSetup);
  
  Serial.printf("[SETTINGS_MANAGER] Settings saved successfully\n");
  return true;
}

// Reset to default settings
void SettingsManager::resetToDefaults() {
  Serial.println("[SETTINGS_MANAGER] Resetting to default settings");
  currentSettings = defaultSettings;
  saveSettings();
}

// Getters
ClockSettings SettingsManager::getSettings() const {
  return currentSettings;
}

int SettingsManager::getTimezoneOffset() const {
  return currentSettings.timezoneOffset;
}

DSTRules SettingsManager::getDSTRules() const {
  return currentSettings.dstRules;
}

int SettingsManager::getBrightness() const {
  return currentSettings.brightness;
}

bool SettingsManager::isFirstTimeSetup() const {
  return currentSettings.firstTimeSetup;
}

// Setters
void SettingsManager::setTimezoneOffset(int offset) {
  if (isValidTimezoneOffset(offset)) {
    currentSettings.timezoneOffset = offset;
    Serial.printf("[SETTINGS_MANAGER] Timezone offset set to %d\n", offset);
  } else {
    Serial.printf("[SETTINGS_MANAGER] Invalid timezone offset %d ignored\n", offset);
  }
}

void SettingsManager::setDSTRules(DSTRules rules) {
  if (rules >= DST_DISABLED && rules <= DST_EU) {
    currentSettings.dstRules = rules;
    Serial.printf("[SETTINGS_MANAGER] DST rules set to %d\n", rules);
  } else {
    Serial.printf("[SETTINGS_MANAGER] Invalid DST rules %d ignored\n", rules);
  }
}

void SettingsManager::setBrightness(int brightness) {
  if (isValidBrightness(brightness)) {
    currentSettings.brightness = brightness;
    Serial.printf("[SETTINGS_MANAGER] Brightness set to %d\n", brightness);
  } else {
    Serial.printf("[SETTINGS_MANAGER] Invalid brightness %d ignored\n", brightness);
  }
}

void SettingsManager::setFirstTimeSetup(bool firstTime) {
  currentSettings.firstTimeSetup = firstTime;
  Serial.printf("[SETTINGS_MANAGER] First time setup set to %s\n", firstTime ? "true" : "false");
}

// Timezone helpers
String SettingsManager::getTimezoneDisplayName(int offset) const {
  for (int i = 0; i < timezoneCount; i++) {
    if (timezones[i].offset == offset) {
      return String(timezones[i].name);
    }
  }
  
  // If not found in predefined list, create generic name
  if (offset >= 0) {
    return "UTC+" + String(offset);
  } else {
    return "UTC" + String(offset);
  }
}

String SettingsManager::getCurrentTimezoneDisplayName() const {
  return getTimezoneDisplayName(currentSettings.timezoneOffset);
}

int SettingsManager::getNextTimezoneOffset(int currentOffset) const {
  for (int i = 0; i < timezoneCount - 1; i++) {
    if (timezones[i].offset == currentOffset) {
      return timezones[i + 1].offset;
    }
  }
  // If at end or not found, wrap to beginning
  return timezones[0].offset;
}

int SettingsManager::getPreviousTimezoneOffset(int currentOffset) const {
  for (int i = 1; i < timezoneCount; i++) {
    if (timezones[i].offset == currentOffset) {
      return timezones[i - 1].offset;
    }
  }
  // If at beginning or not found, wrap to end
  return timezones[timezoneCount - 1].offset;
}

// DST helpers
String SettingsManager::getDSTRulesDisplayName(DSTRules rules) const {
  switch (rules) {
    case DST_DISABLED: return "Disabled";
    case DST_US: return "US Rules";
    case DST_EU: return "EU Rules";
    default: return "Unknown";
  }
}

DSTRules SettingsManager::getNextDSTRules(DSTRules currentRules) const {
  switch (currentRules) {
    case DST_DISABLED: return DST_US;
    case DST_US: return DST_EU;
    case DST_EU: return DST_DISABLED;
    default: return DST_DISABLED;
  }
}

// Validation
bool SettingsManager::isValidTimezoneOffset(int offset) const {
  return (offset >= -12 && offset <= 12);
}

bool SettingsManager::isValidBrightness(int brightness) const {
  return (brightness >= 0 && brightness <= 100);
}

// Status
String SettingsManager::getStatusString() const {
  String status = "TZ: " + getCurrentTimezoneDisplayName();
  status += ", DST: " + getDSTRulesDisplayName(currentSettings.dstRules);
  status += ", Brightness: " + String(currentSettings.brightness) + "%";
  return status;
}
