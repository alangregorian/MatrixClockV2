#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_NeoMatrix.h>
#include "wifi_manager.h"
#include "display_manager.h"
#include "button_handler.h"

// System State Machine
enum SystemState {
  STATE_INIT,
  STATE_LOGO_DISPLAY,    // Show startup logo for 4 seconds
  STATE_STARTUP_MESSAGE, // Show startup message before WiFi scan
  STATE_WIFI_SCAN,
  STATE_WIFI_DISPLAY,
  STATE_PASSWORD_ENTRY,  // Password entry for selected network
  STATE_WIFI_CONNECT,    // Future expansion
  STATE_WIFI_CONNECT_FAILED, // Display connection failure for 2 seconds
  STATE_TIME_SYNC,       // Future expansion  
  STATE_CLOCK_DISPLAY    // Future expansion
};

class StateMachine {
private:
  // State variables
  SystemState currentState;
  SystemState previousState;
  bool displayNeedsUpdate;
  bool stateChanged;
  
  // Password entry variables
  String currentPassword;
  int passwordPosition;
  int currentCharIndex;
  static const char characterSet[];
  static const int characterSetSize;
  
  // Hardware references
  Adafruit_ST7789& tft;
  Adafruit_NeoMatrix& matrix;

public:
  // Constructor
  StateMachine(Adafruit_ST7789& tftDisplay, Adafruit_NeoMatrix& neoMatrix);
  
  // State management methods
  void changeState(SystemState newState);
  SystemState getCurrentState() const;
  SystemState getPreviousState() const;
  bool hasStateChanged() const;
  bool needsDisplayUpdate() const;
  void clearStateChanged();
  void setDisplayNeedsUpdate(bool needsUpdate = true);
  
  // Main state machine update method
  void update();
  
  // State-specific handlers
  void handleInitState();
  void handleLogoDisplayState();
  void handleStartupMessageState();
  void handleWiFiScanState();
  void handleWiFiDisplayState();
  void handlePasswordEntryState();
  void handleWiFiConnectState();
  void handleWiFiConnectFailedState();
  void handleTimeSyncState();
  void handleClockDisplayState();
};

#endif // STATE_MACHINE_H
