#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_NeoMatrix.h>
#include "wifi_manager.h"
#include "display_manager.h"
#include "button_handler.h"
#include "time_manager.h"
#include "settings_manager.h"

// System State Machine
enum SystemState {
  STATE_INIT,
  STATE_LOGO_DISPLAY,    // Show startup logo for 4 seconds
  STATE_STARTUP_MESSAGE, // Show startup message before WiFi scan
  STATE_WIFI_SCAN,
  STATE_WIFI_DISPLAY,
  STATE_PASSWORD_ENTRY,  // Password entry for selected network
  STATE_WIFI_CONNECTING, // Show "Connecting..." message during connection attempt
  STATE_WIFI_SUCCESS,    // Show successful connection screen
  STATE_WIFI_FAILURE,    // Show connection failure screen with retry options
  STATE_WIFI_CONNECT,    // Future expansion
  STATE_SETTINGS,        // Settings configuration (timezone, DST, brightness)
  STATE_TIME_SYNC,       // Time synchronization
  STATE_CLOCK_DISPLAY,   // Main clock display
  STATE_WORDCLOCK_DISPLAY // WordClock matrix display mode
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
  void handleWiFiConnectingState();
  void handleWiFiSuccessState();
  void handleWiFiFailureState();
  void handleWiFiConnectState();
  void handleSettingsState();
  void handleTimeSyncState();
  void handleClockDisplayState();
  void handleWordClockDisplayState();
};

#endif // STATE_MACHINE_H
