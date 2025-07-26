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
  STATE_WIFI_SCAN,
  STATE_WIFI_DISPLAY,
  STATE_WIFI_CONNECT,    // Future expansion
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
  void handleWiFiScanState();
  void handleWiFiDisplayState();
  void handleWiFiConnectState();
  void handleTimeSyncState();
  void handleClockDisplayState();
};

#endif // STATE_MACHINE_H
