/*
   ESP32-S2 WordClock Migration - Main State Machine
   Based on working ESP32_WordClock_Migration.ino
   
   Hardware:
   - Adafruit ESP32-S2 Reverse TFT Feather
   - Built-in 240x135 ST7789 TFT display
   - Built-in WiFi
   - 3 tactile buttons (D0, D1, D2)
   - Optional: 8x8 NeoMatrix connected to pin 6
*/

#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// Include our modular headers
#include "include/wifi_manager.h"
#include "include/display_manager.h"
#include "include/button_handler.h"

// System State Machine
enum SystemState {
  STATE_INIT,
  STATE_WIFI_SCAN,
  STATE_WIFI_DISPLAY,
  STATE_WIFI_CONNECT,    // Future expansion
  STATE_TIME_SYNC,       // Future expansion  
  STATE_CLOCK_DISPLAY    // Future expansion
};

// Global state
SystemState currentState = STATE_INIT;
SystemState previousState = STATE_INIT;

// TFT Display pins are predefined by ESP32-S2 Reverse TFT Feather board variant
// TFT_CS = 42, TFT_RST = 41, TFT_DC = 40, TFT_MOSI = 35, TFT_SCLK = 36, TFT_BACKLIGHT = 45

// Button pins
const int BUTTON_A = 0;       // D0 (also BOOT button)
const int BUTTON_B = 1;       // D1
const int BUTTON_C = 2;       // D2

// NeoMatrix pin
const int NEOPIN = 6;

// Global hardware objects
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, NEOPIN,
                            NEO_MATRIX_TOP  + NEO_MATRIX_LEFT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB         + NEO_KHZ800);

// State transition function
void changeState(SystemState newState) {
  if (newState != currentState) {
    previousState = currentState;
    currentState = newState;
    Serial.printf("State transition: %d -> %d\n", previousState, currentState);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32-S2 WordClock Migration - State Machine");
  Serial.println("============================================");
  
  // Initialize hardware
  initializeButtons();
  initializeDisplay(tft, matrix);
  
  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  // Show startup message
  displayStartupMessage(tft);
  
  // Initialize NeoMatrix startup pattern
  showStartupPattern(matrix);
  
  delay(2000);
  
  // Transition to WiFi scanning state
  changeState(STATE_WIFI_SCAN);
}

void loop() {
  // Handle button inputs (always active)
  ButtonEvent buttonEvent = handleButtons();
  
  // State machine
  switch (currentState) {
    case STATE_INIT:
      // Initialization complete, move to WiFi scan
      changeState(STATE_WIFI_SCAN);
      break;
      
    case STATE_WIFI_SCAN:
      // Perform WiFi scan
      scanWiFiNetworks(tft, matrix);
      changeState(STATE_WIFI_DISPLAY);
      break;
      
    case STATE_WIFI_DISPLAY:
      // Display current network and handle navigation
      if (getNetworkCount() > 0) {
        displayCurrentNetwork(tft, matrix);
        
        // Handle button events
        switch (buttonEvent) {
          case BUTTON_A_PRESSED:
            nextNetwork();
            break;
          case BUTTON_B_PRESSED:
            previousNetwork();
            break;
          case BUTTON_C_PRESSED:
            changeState(STATE_WIFI_SCAN);  // Rescan
            break;
          case NO_BUTTON:
            // Stay in display state
            break;
        }
      }
      break;
      
    case STATE_WIFI_CONNECT:
      // Future: Connect to selected WiFi network
      Serial.println("WiFi Connect state - Not implemented yet");
      changeState(STATE_WIFI_DISPLAY);
      break;
      
    case STATE_TIME_SYNC:
      // Future: NTP time synchronization
      Serial.println("Time Sync state - Not implemented yet");
      changeState(STATE_CLOCK_DISPLAY);
      break;
      
    case STATE_CLOCK_DISPLAY:
      // Future: WordClock display mode
      Serial.println("Clock Display state - Not implemented yet");
      changeState(STATE_WIFI_DISPLAY);
      break;
  }
  
  delay(50); // Small delay to prevent excessive CPU usage
}
