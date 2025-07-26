#include "../include/state_machine.h"

// Constructor
StateMachine::StateMachine(Adafruit_ST7789& tftDisplay, Adafruit_NeoMatrix& neoMatrix)
  : tft(tftDisplay), matrix(neoMatrix), currentState(STATE_INIT), previousState(STATE_INIT), 
    displayNeedsUpdate(true), stateChanged(false) {
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
      
    case STATE_WIFI_SCAN:
      handleWiFiScanState();
      break;
      
    case STATE_WIFI_DISPLAY:
      handleWiFiDisplayState();
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
  // Initialization complete, move to WiFi scan
  changeState(STATE_WIFI_SCAN);
}

void StateMachine::handleWiFiScanState() {
  // Perform WiFi scan only once per state entry
  if (stateChanged || displayNeedsUpdate) {
    scanWiFiNetworks();
    stateChanged = false;
    displayNeedsUpdate = true; // Will need to display results
  }
  changeState(STATE_WIFI_DISPLAY);
}

void StateMachine::handleWiFiDisplayState() {
  // Handle button inputs
  ButtonEvent buttonEvent = handleButtons();
  
  // Display current network and handle navigation
  if (getNetworkCount() > 0) {
    // Only update display when needed
    if (displayNeedsUpdate || stateChanged) {
      // Get current network info from wifi_manager
      WiFiNetworkInfo networkInfo = getCurrentNetworkInfo();
      // Display it using display_manager (function needs to be implemented)
      // displayCurrentNetwork(tft, matrix, networkInfo);
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
        previousNetwork();
        displayNeedsUpdate = true;  // Network changed, need to update display
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

void StateMachine::handleClockDisplayState() {
  // Future: WordClock display mode
  Serial.println("Clock Display state - Not implemented yet");
  changeState(STATE_WIFI_DISPLAY);
}
