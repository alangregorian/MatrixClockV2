#include "../include/state_machine.h"
#include "../include/wifi_manager.h"

// Character set for password entry
const char StateMachine::characterSet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>? ";
const int StateMachine::characterSetSize = sizeof(StateMachine::characterSet) - 1; // -1 to exclude null terminator

// Constructor
StateMachine::StateMachine(Adafruit_ST7789& tftDisplay, Adafruit_NeoMatrix& neoMatrix)
  : tft(tftDisplay), matrix(neoMatrix), currentState(STATE_INIT), previousState(STATE_INIT), 
    displayNeedsUpdate(true), stateChanged(false), currentPassword(""), passwordPosition(0), currentCharIndex(0) {
}

// State management methods
void StateMachine::changeState(SystemState newState) {
  if (newState != currentState) {
    previousState = currentState;
    currentState = newState;
    stateChanged = true;
    displayNeedsUpdate = true;
    Serial.printf("State transition: %d -> %d\n", previousState, currentState);
  }
}

void StateMachine::handleWordClockDisplayState() {
  Serial.printf("[STATE_WORDCLOCK_DISPLAY] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Initialize WordClock on first entry
  static bool wordClockInitialized = false;
  if (stateChanged && !wordClockInitialized) {
    Serial.println("[WORDCLOCK_DISPLAY] Initializing WordClock...");
    showWordClockStartup(matrix);
    wordClockInitialized = true;
  }
  
  // Update display when needed
  static unsigned long lastDisplayUpdate = 0;
  unsigned long currentTime = millis();
  
  if (stateChanged || displayNeedsUpdate || (currentTime - lastDisplayUpdate > 1000)) {
    // Get current time from time manager
    if (timeManager.getSyncStatus() == TIME_SYNC_SUCCESS) {
      // Create a tm struct from TimeManager data
      struct tm timeinfo;
      timeinfo.tm_hour = timeManager.getHours();
      timeinfo.tm_min = timeManager.getMinutes();
      timeinfo.tm_sec = timeManager.getSeconds();
      timeinfo.tm_mday = timeManager.getDay();
      timeinfo.tm_mon = timeManager.getMonth() - 1; // tm_mon is 0-based
      timeinfo.tm_year = timeManager.getYear() - 1900; // tm_year is years since 1900
      
      // Display WordClock mode (TFT shows status, matrix shows time as words)
      displayWordClockMode(tft, matrix, &timeinfo);
      
      lastDisplayUpdate = currentTime;
      stateChanged = false;
      displayNeedsUpdate = false;
      
      Serial.printf("[WORDCLOCK_DISPLAY] Time: %02d:%02d:%02d\n", 
                    timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    } else {
      // Time not synced, show error on TFT
      clearTFTScreen(tft);
      tft.setTextColor(ST77XX_RED);
      tft.setTextSize(2);
      tft.setCursor(10, 50);
      tft.println("Time Not Synced");
      tft.setTextSize(1);
      tft.setCursor(10, 80);
      tft.println("Press B to sync time");
    }
  }
  
  // Handle button inputs for navigation and settings
  ButtonEvent buttonEvent = handleButtons();
  switch (buttonEvent) {
    case BUTTON_A_PRESSED:
      // Go to settings menu
      Serial.println("Settings button pressed - going to settings");
      changeState(STATE_SETTINGS);
      break;
      
    case BUTTON_B_PRESSED:
      // Force time sync
      Serial.println("Force sync button pressed");
      if (timeManager.forceSync()) {
        Serial.println("Time sync successful!");
        displayNeedsUpdate = true;
      } else {
        Serial.println("Time sync failed!");
        displayNeedsUpdate = true;
      }
      break;
      
    case BUTTON_C_PRESSED:
      // Switch back to regular clock display
      Serial.println("Switching to regular clock display");
      changeState(STATE_CLOCK_DISPLAY);
      break;
      
    case NO_BUTTON:
      // Stay in WordClock display state
      break;
  }
  
  // Check if time sync is needed periodically
  if (timeManager.needsTimeSync()) {
    Serial.println("[WORDCLOCK_DISPLAY] Time sync needed, attempting sync...");
    if (timeManager.syncTime()) {
      Serial.println("[WORDCLOCK_DISPLAY] Background sync successful");
      displayNeedsUpdate = true;
    } else {
      Serial.println("[WORDCLOCK_DISPLAY] Background sync failed");
    }
  }
}

SystemState StateMachine::getCurrentState() const {
  return currentState;
}

SystemState StateMachine::getPreviousState() const {
  return previousState;
}

bool StateMachine::hasStateChanged() const {
  return stateChanged;
}

bool StateMachine::needsDisplayUpdate() const {
  return displayNeedsUpdate;
}

void StateMachine::clearStateChanged() {
  stateChanged = false;
}

void StateMachine::setDisplayNeedsUpdate(bool needsUpdate) {
  displayNeedsUpdate = needsUpdate;
}

// Main state machine update method
void StateMachine::update() {
  switch (currentState) {
    case STATE_INIT:
      handleInitState();
      break;
      
    case STATE_LOGO_DISPLAY:
      handleLogoDisplayState();
      break;
      
    case STATE_STARTUP_MESSAGE:
      handleStartupMessageState();
      break;
      
    case STATE_WIFI_SCAN:
      handleWiFiScanState();
      break;
      
    case STATE_WIFI_DISPLAY:
      handleWiFiDisplayState();
      break;
      
    case STATE_PASSWORD_ENTRY:
      handlePasswordEntryState();
      break;
      
    case STATE_WIFI_CONNECTING:
      handleWiFiConnectingState();
      break;
      
    case STATE_WIFI_SUCCESS:
      handleWiFiSuccessState();
      break;
      
    case STATE_WIFI_FAILURE:
      handleWiFiFailureState();
      break;
      
    case STATE_WIFI_CONNECT:
      handleWiFiConnectState();
      break;
      
    case STATE_SETTINGS:
      handleSettingsState();
      break;
      
    case STATE_TIME_SYNC:
      handleTimeSyncState();
      break;
      
    case STATE_CLOCK_DISPLAY:
      handleClockDisplayState();
      break;
      
    case STATE_WORDCLOCK_DISPLAY:
      handleWordClockDisplayState();
      break;
  }
}

// State-specific handlers
void StateMachine::handleInitState() {
  Serial.printf("[STATE_INIT] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Initialization complete, move to logo display
  changeState(STATE_LOGO_DISPLAY);
}

void StateMachine::handleLogoDisplayState() {
  Serial.printf("[STATE_LOGO_DISPLAY] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Display logo only once when entering this state
  if (stateChanged || displayNeedsUpdate) {
    displayClockLogo(tft);
    stateChanged = false;
    displayNeedsUpdate = false;
  }
  
  // Use static variable to track timing for this state
  static unsigned long logoStartTime = 0;
  
  // Initialize timer on first entry
  if (logoStartTime == 0) {
    logoStartTime = millis();
  }
  
  // Check if 4 seconds have elapsed
  if (millis() - logoStartTime >= 4000) {
    logoStartTime = 0; // Reset for next time
    changeState(STATE_STARTUP_MESSAGE);
  }
}

void StateMachine::handleStartupMessageState() {
  Serial.printf("[STATE_STARTUP_MESSAGE] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Display startup message only once when entering this state
  if (stateChanged || displayNeedsUpdate) {
    displayStartupMessage(tft);
    stateChanged = false;
    displayNeedsUpdate = false;
  }
  
  // Use static variable to track timing for this state
  static unsigned long messageStartTime = 0;
  
  // Initialize timer on first entry
  if (messageStartTime == 0) {
    messageStartTime = millis();
  }
  
  // Check if 3 seconds have elapsed
  if (millis() - messageStartTime >= 3000) {
    messageStartTime = 0; // Reset for next time
    changeState(STATE_WIFI_SCAN);
  }
}

void StateMachine::handleWiFiScanState() {
  Serial.printf("[STATE_WIFI_SCAN] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Perform WiFi scan only once per state entry
  if (stateChanged || displayNeedsUpdate) {
    Serial.printf("[WIFI_SCAN] Before scan - Free Heap: %d\n", ESP.getFreeHeap());
    scanWiFiNetworks();
    Serial.printf("[WIFI_SCAN] After scan - Free Heap: %d\n", ESP.getFreeHeap());
    stateChanged = false;
    displayNeedsUpdate = true; // Will need to display results
  }
  changeState(STATE_WIFI_DISPLAY);
}

void StateMachine::handleWiFiDisplayState() {
  Serial.printf("[STATE_WIFI_DISPLAY] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Handle button inputs
  ButtonEvent buttonEvent = handleButtons();
  
  // Display current network and handle navigation
  if (getNetworkCount() > 0) {
    // Only update display when needed
    if (displayNeedsUpdate || stateChanged) {
      Serial.printf("[WIFI_DISPLAY] Before display - Free Heap: %d\n", ESP.getFreeHeap());
      // Get current network info from wifi_manager
      WiFiNetworkInfo networkInfo = getCurrentNetworkInfo();
      // Display it using display_manager with current index and total count
      displayCurrentNetwork(tft, matrix, networkInfo, currentNetwork, getNetworkCount());
      Serial.printf("[WIFI_DISPLAY] After display - Free Heap: %d\n", ESP.getFreeHeap());
      displayNeedsUpdate = false;
      stateChanged = false;
    }
    
    // Handle button events
    switch (buttonEvent) {
      case BUTTON_A_PRESSED:
        nextNetwork();
        displayNeedsUpdate = true;  // Network changed, need to update display
        break;
      case BUTTON_B_PRESSED:
        {
          selectCurrentNetwork();
          // Check if network is open (no password required)
          WiFiNetworkInfo networkInfo = getCurrentNetworkInfo();
          if (networkInfo.encryption == WIFI_AUTH_OPEN) {
            // Open network - set empty password and go to connecting state
            Serial.println("Open network detected - connecting directly");
            currentPassword = "";
            changeState(STATE_WIFI_CONNECTING);
          } else {
            // Secured network - go to password entry
            currentPassword = "";
            passwordPosition = 0;
            currentCharIndex = 0;
            changeState(STATE_PASSWORD_ENTRY);
          }
        }
        break;
      case BUTTON_C_PRESSED:
        changeState(STATE_WIFI_SCAN);  // Rescan
        break;
      case NO_BUTTON:
        // Stay in display state - no update needed
        break;
    }
  }
}

void StateMachine::handleWiFiConnectState() {
  // Future: Connect to selected WiFi network
  Serial.println("WiFi Connect state - Not implemented yet");
  changeState(STATE_WIFI_DISPLAY);
}

void StateMachine::handleTimeSyncState() {
  Serial.printf("[STATE_TIME_SYNC] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Initialize time manager on first entry
  static bool timeManagerInitialized = false;
  if (stateChanged || displayNeedsUpdate) {
    if (!timeManagerInitialized) {
      Serial.println("[TIME_SYNC] Initializing time manager...");
      if (timeManager.initialize()) {
        timeManagerInitialized = true;
        Serial.println("[TIME_SYNC] Time manager initialized successfully");
      } else {
        Serial.println("[TIME_SYNC] Failed to initialize time manager");
        changeState(STATE_WIFI_FAILURE);
        return;
      }
    }
    
    // Display sync status
    displayTimeSyncStatus(tft, timeManager.getStatusString());
    stateChanged = false;
    displayNeedsUpdate = false;
  }
  
  // Attempt time sync if not already synced
  if (timeManager.getSyncStatus() == TIME_NOT_SYNCED || 
      timeManager.getSyncStatus() == TIME_SYNC_FAILED) {
    
    Serial.println("[TIME_SYNC] Attempting NTP sync...");
    if (timeManager.syncTime()) {
      Serial.println("[TIME_SYNC] Time sync successful!");
      // Brief delay to show success message
      delay(1000);
      changeState(STATE_CLOCK_DISPLAY);
    } else {
      Serial.println("[TIME_SYNC] Time sync failed!");
      // Show failure for a moment, then retry or continue
      displayTimeSyncStatus(tft, "Sync Failed");
      delay(2000);
      
      // For now, continue to clock display even if sync failed
      // In the future, could add retry logic or fallback
      changeState(STATE_CLOCK_DISPLAY);
    }
  } else if (timeManager.getSyncStatus() == TIME_SYNC_SUCCESS) {
    // Already synced, move to clock display
    changeState(STATE_CLOCK_DISPLAY);
  }
}

void StateMachine::handlePasswordEntryState() {
  Serial.printf("[STATE_PASSWORD_ENTRY] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Handle button inputs
  ButtonEvent buttonEvent = handleButtons();
  
  // Display password entry screen when needed
  if (displayNeedsUpdate || stateChanged) {
    displayPasswordEntry(tft, matrix, getSelectedSSID(), currentPassword, characterSet[currentCharIndex]);
    
    displayNeedsUpdate = false;
    stateChanged = false;
  }
  
  // Handle button events
  switch (buttonEvent) {
    case BUTTON_A_PRESSED:
      // Cycle through characters
      currentCharIndex = (currentCharIndex + 1) % characterSetSize;
      displayNeedsUpdate = true;
      Serial.printf("Character changed to: %c\n", characterSet[currentCharIndex]);
      break;
      
    case BUTTON_B_PRESSED:
      // Confirm current character and add to password
      currentPassword += characterSet[currentCharIndex];
      passwordPosition++;
      currentCharIndex = 0; // Reset to 'a' for next character
      displayNeedsUpdate = true;
      Serial.printf("Character confirmed. Password length: %d\n", currentPassword.length());
      break;
      
    case BUTTON_C_PRESSED:
      // Submit password and transition to connecting state
      Serial.printf("Submitting password for %s\n", getSelectedSSID().c_str());
      changeState(STATE_WIFI_CONNECTING);
      break;
      
    case NO_BUTTON:
      // Stay in password entry state
      break;
  }
}

void StateMachine::handleWiFiConnectingState() {
  Serial.printf("[STATE_WIFI_CONNECTING] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Display connecting message when entering this state
  if (stateChanged || displayNeedsUpdate) {
    displayConnectingMessage(tft, getSelectedSSID());
    stateChanged = false;
    displayNeedsUpdate = false;
  }
  
  // Attempt WiFi connection
  Serial.printf("Attempting to connect to: %s\n", getSelectedSSID().c_str());
  if (connectToNetwork(getSelectedSSID(), currentPassword)) {
    Serial.println("WiFi connection successful!");
    changeState(STATE_WIFI_SUCCESS);
  } else {
    Serial.println("WiFi connection failed!");
    changeState(STATE_WIFI_FAILURE);
  }
}

void StateMachine::handleWiFiSuccessState() {
  Serial.printf("[STATE_WIFI_SUCCESS] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Display success message when entering this state
  if (stateChanged || displayNeedsUpdate) {
    // Get connection details
    String ipAddress = WiFi.localIP().toString();
    WiFiNetworkInfo networkInfo = getCurrentNetworkInfo();
    
    displayWiFiSuccess(tft, getSelectedSSID(), ipAddress, networkInfo.rssi);
    stateChanged = false;
    displayNeedsUpdate = false;
  }
  
  // Handle button inputs - any button continues to next state
  ButtonEvent buttonEvent = handleButtons();
  if (buttonEvent != NO_BUTTON) {
    Serial.println("Button pressed - continuing to settings");
    // Initialize settings manager if this is first time setup
    if (!settingsManager.initialize()) {
      Serial.println("Failed to initialize settings manager, using defaults");
    }
    
    // Check if this is first time setup
    if (settingsManager.isFirstTimeSetup()) {
      Serial.println("First time setup - going to settings");
      changeState(STATE_SETTINGS);
    } else {
      Serial.println("Settings already configured - going to time sync");
      changeState(STATE_TIME_SYNC);
    }
  }
}

void StateMachine::handleWiFiFailureState() {
  Serial.printf("[STATE_WIFI_FAILURE] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Display failure message when entering this state
  if (stateChanged || displayNeedsUpdate) {
    displayWiFiFailure(tft, getSelectedSSID());
    stateChanged = false;
    displayNeedsUpdate = false;
  }
  
  // Handle button inputs for retry options
  ButtonEvent buttonEvent = handleButtons();
  switch (buttonEvent) {
    case BUTTON_A_PRESSED:
      // Retry with same password
      Serial.println("Retrying connection with same password");
      changeState(STATE_WIFI_CONNECTING);
      break;
      
    case BUTTON_B_PRESSED:
      // Enter new password
      Serial.println("Entering new password");
      currentPassword = "";
      passwordPosition = 0;
      currentCharIndex = 0;
      changeState(STATE_PASSWORD_ENTRY);
      break;
      
    case BUTTON_C_PRESSED:
      // Go back to network selection
      Serial.println("Going back to network selection");
      changeState(STATE_WIFI_DISPLAY);
      break;
      
    case NO_BUTTON:
      // Stay in failure state
      break;
  }
}

void StateMachine::handleSettingsState() {
  Serial.printf("[STATE_SETTINGS] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Settings menu variables (static to persist across calls)
  static int currentSettingIndex = 0;
  static const int numSettings = 3; // Timezone, DST Rules, Brightness
  
  // Handle button inputs
  ButtonEvent buttonEvent = handleButtons();
  
  // Display settings menu when needed
  if (displayNeedsUpdate || stateChanged) {
    // Get current settings
    ClockSettings settings = settingsManager.getSettings();
    
    // Create settings display strings
    String timezoneStr = "Timezone: " + settingsManager.getCurrentTimezoneDisplayName();
    String dstStr = "DST: " + settingsManager.getDSTRulesDisplayName(settings.dstRules);
    String brightnessStr = "Brightness: " + String(settings.brightness) + "%";
    String saveStr = "Save & Exit";
    
    // Display settings menu (this function needs to be added to display_manager)
    displaySettingsMenu(tft, currentSettingIndex, timezoneStr, dstStr, brightnessStr, saveStr);
    
    displayNeedsUpdate = false;
    stateChanged = false;
  }
  
  // Handle button events
  switch (buttonEvent) {
    case BUTTON_A_PRESSED:
      // Navigate to next setting
      currentSettingIndex = (currentSettingIndex + 1) % (numSettings + 1); // +1 for Save & Exit
      displayNeedsUpdate = true;
      Serial.printf("Settings navigation: index %d\n", currentSettingIndex);
      break;
      
    case BUTTON_B_PRESSED:
      // Modify current setting
      switch (currentSettingIndex) {
        case 0: // Timezone
          {
            int currentOffset = settingsManager.getTimezoneOffset();
            int nextOffset = settingsManager.getNextTimezoneOffset(currentOffset);
            settingsManager.setTimezoneOffset(nextOffset);
            Serial.printf("Timezone changed to: %s\n", settingsManager.getCurrentTimezoneDisplayName().c_str());
          }
          break;
          
        case 1: // DST Rules
          {
            DSTRules currentRules = settingsManager.getDSTRules();
            DSTRules nextRules = settingsManager.getNextDSTRules(currentRules);
            settingsManager.setDSTRules(nextRules);
            Serial.printf("DST rules changed to: %s\n", settingsManager.getDSTRulesDisplayName(nextRules).c_str());
          }
          break;
          
        case 2: // Brightness
          {
            int currentBrightness = settingsManager.getBrightness();
            int newBrightness = (currentBrightness + 25) % 125; // 0, 25, 50, 75, 100
            if (newBrightness == 0) newBrightness = 25; // Skip 0, start at 25
            settingsManager.setBrightness(newBrightness);
            Serial.printf("Brightness changed to: %d%%\n", newBrightness);
          }
          break;
          
        case 3: // Save & Exit
          // Save settings and exit
          Serial.println("Saving settings and exiting...");
          settingsManager.setFirstTimeSetup(false); // Mark setup as complete
          if (settingsManager.saveSettings()) {
            Serial.println("Settings saved successfully");
            changeState(STATE_TIME_SYNC);
          } else {
            Serial.println("Failed to save settings");
            // Still continue to time sync
            changeState(STATE_TIME_SYNC);
          }
          break;
      }
      displayNeedsUpdate = true;
      break;
      
    case BUTTON_C_PRESSED:
      // Cancel/Exit without saving (if coming from clock display)
      if (previousState == STATE_CLOCK_DISPLAY) {
        Serial.println("Canceling settings changes - returning to clock");
        // Reload settings to discard changes
        settingsManager.loadSettings();
        changeState(STATE_CLOCK_DISPLAY);
      } else {
        // If coming from first-time setup, force save and continue
        Serial.println("First-time setup - saving and continuing");
        settingsManager.setFirstTimeSetup(false);
        settingsManager.saveSettings();
        changeState(STATE_TIME_SYNC);
      }
      break;
      
    case NO_BUTTON:
      // Stay in settings state
      break;
  }
}

void StateMachine::handleClockDisplayState() {
  Serial.printf("[STATE_CLOCK_DISPLAY] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  
  // Update display when needed
  static unsigned long lastDisplayUpdate = 0;
  unsigned long currentTime = millis();
  
  if (stateChanged || displayNeedsUpdate || (currentTime - lastDisplayUpdate > 1000)) {
    // Get current time and date from time manager
    String timeString = timeManager.getFormattedTime();
    String dateString = timeManager.getFormattedDate();
    String statusString = timeManager.getStatusString();
    
    // Display the clock screen
    displayClockScreen(tft, timeString, dateString, statusString);
    
    lastDisplayUpdate = currentTime;
    stateChanged = false;
    displayNeedsUpdate = false;
    
    Serial.printf("[CLOCK_DISPLAY] Time: %s, Date: %s, Status: %s\n", 
                  timeString.c_str(), dateString.c_str(), statusString.c_str());
  }
  
  // Handle button inputs for navigation and settings
  ButtonEvent buttonEvent = handleButtons();
  switch (buttonEvent) {
    case BUTTON_A_PRESSED:
      // Go to settings menu
      Serial.println("Settings button pressed - going to settings");
      changeState(STATE_SETTINGS);
      break;
      
    case BUTTON_B_PRESSED:
      // Force time sync
      Serial.println("Force sync button pressed");
      if (timeManager.forceSync()) {
        Serial.println("Time sync successful!");
        displayNeedsUpdate = true;
      } else {
        Serial.println("Time sync failed!");
        displayNeedsUpdate = true;
      }
      break;
      
    case BUTTON_C_PRESSED:
      // Switch to WordClock display mode
      Serial.println("WordClock mode button pressed");
      changeState(STATE_WORDCLOCK_DISPLAY);
      break;
      
    case NO_BUTTON:
      // Stay in clock display state
      break;
  }
  
  // Check if time sync is needed periodically
  if (timeManager.needsTimeSync()) {
    Serial.println("[CLOCK_DISPLAY] Time sync needed, attempting sync...");
    if (timeManager.syncTime()) {
      Serial.println("[CLOCK_DISPLAY] Background sync successful");
      displayNeedsUpdate = true;
    } else {
      Serial.println("[CLOCK_DISPLAY] Background sync failed");
    }
  }
}
