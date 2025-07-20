#include "../include/button_handler.h"

// Button state tracking
bool buttonA_pressed = false;
bool buttonB_pressed = false;
bool buttonC_pressed = false;
unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_DELAY = 200;

void initializeButtons() {
  // Initialize buttons
  pinMode(BUTTON_A, INPUT_PULLUP);  // D0: pulled HIGH, goes LOW when pressed
  pinMode(BUTTON_B, INPUT);         // D1: pulled LOW by hardware, goes HIGH when pressed
  pinMode(BUTTON_C, INPUT);         // D2: pulled LOW by hardware, goes HIGH when pressed
  
  Serial.println("Buttons initialized");
}

ButtonEvent handleButtons() {
  unsigned long currentTime = millis();
  
  // Debug: Print button states every 2 seconds
  static unsigned long lastDebug = 0;
  if (currentTime - lastDebug > 2000) {
    Serial.printf("Button states - D0:%d D1:%d D2:%d\n", 
                  digitalRead(BUTTON_A), digitalRead(BUTTON_B), digitalRead(BUTTON_C));
    lastDebug = currentTime;
  }
  
  // Debounce check
  if (currentTime - lastButtonPress < DEBOUNCE_DELAY) {
    return NO_BUTTON;
  }
  
  // Button A / D0 (Down/Next)
  if (digitalRead(BUTTON_A) == LOW && !buttonA_pressed) {
    buttonA_pressed = true;
    lastButtonPress = currentTime;
    Serial.printf("Button D0: Next network\n");
    return BUTTON_A_PRESSED;
  } else if (digitalRead(BUTTON_A) == HIGH) {
    buttonA_pressed = false;
  }
  
  // Button B / D1 (Up/Previous) - D1 goes HIGH when pressed
  if (digitalRead(BUTTON_B) == HIGH && !buttonB_pressed) {
    buttonB_pressed = true;
    lastButtonPress = currentTime;
    Serial.printf("Button D1: Previous network\n");
    return BUTTON_B_PRESSED;
  } else if (digitalRead(BUTTON_B) == LOW) {
    buttonB_pressed = false;
  }
  
  // Button C / D2 (Rescan) - D2 goes HIGH when pressed
  if (digitalRead(BUTTON_C) == HIGH && !buttonC_pressed) {
    buttonC_pressed = true;
    lastButtonPress = currentTime;
    Serial.println("Button D2: Rescanning WiFi networks");
    return BUTTON_C_PRESSED;
  } else if (digitalRead(BUTTON_C) == LOW) {
    buttonC_pressed = false;
  }
  
  return NO_BUTTON;
}
