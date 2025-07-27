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
      
    case STATE_TIME_SYNC:
      handleTimeSyncState();
      break;
      
    case STATE_CLOCK_DISPLAY:
      handleClockDisplayState();
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
  // Future: NTP time synchronization
  Serial.println("Time Sync state - Not implemented yet");
  changeState(STATE_CLOCK_DISPLAY);
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
    Serial.println("Button pressed - continuing to time sync");
    changeState(STATE_TIME_SYNC);
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

void StateMachine::handleClockDisplayState() {
  // Future: WordClock display mode
  Serial.println("Clock Display state - Not implemented yet");
  changeState(STATE_WIFI_DISPLAY);
}
