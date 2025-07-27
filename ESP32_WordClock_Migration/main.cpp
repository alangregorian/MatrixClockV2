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
#include "include/version.h"
#include "include/wifi_manager.h"
#include "include/display_manager.h"
#include "include/button_handler.h"
#include "include/state_machine.h"

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

// State machine instance
StateMachine stateMachine(tft, matrix);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32-S2 WordClock Migration - State Machine");
  Serial.println("============================================");
  Serial.print("Version: ");
  Serial.println(SOFTWARE_VERSION);
  Serial.print("Build: ");
  Serial.print(BUILD_DATE);
  Serial.print(" ");
  Serial.println(BUILD_TIME);
  Serial.println("Features: Logo display, WiFi scan, State machine");
  Serial.println("DEBUG: Setup started");
  Serial.flush();
  
  // Initialize hardware
  Serial.println("DEBUG: About to initialize buttons");
  Serial.flush();
  initializeButtons();
  Serial.println("DEBUG: Buttons initialized successfully");
  Serial.flush();
  
  Serial.println("DEBUG: About to initialize display");
  Serial.flush();
  initializeDisplay(tft, matrix);
  Serial.println("DEBUG: Display initialized successfully");
  Serial.flush();
  
  // Initialize WiFi
  Serial.println("DEBUG: About to initialize WiFi");
  Serial.flush();
  WiFi.mode(WIFI_STA);
  Serial.println("DEBUG: WiFi mode set to STA");
  Serial.flush();
  
  WiFi.disconnect();
  Serial.println("DEBUG: WiFi disconnected");
  Serial.flush();
  
  delay(100);
  Serial.println("DEBUG: WiFi initialization complete");
  Serial.flush();
  
  // State machine will handle logo display automatically
  Serial.println("DEBUG: Setup complete - entering main loop");
  Serial.println("DEBUG: Logo will be displayed by state machine for 4 seconds");
  Serial.printf("[SETUP_COMPLETE] Free Heap: %d, Min Free: %d\n", 
                ESP.getFreeHeap(), ESP.getMinFreeHeap());
  Serial.flush();
}

void loop() {
  // Update the state machine
  stateMachine.update();
  
  delay(50); // Small delay to prevent excessive CPU usage
}
