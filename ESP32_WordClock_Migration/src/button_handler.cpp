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

void initializeUnusedPins() {
  Serial.println("Initializing specific pin headers with pull-down resistors...");
  
  // Conservative approach: Only initialize specific pins that are exposed as headers
  // and causing false button detections when touched
  
  // Analog pins A0-A5 (these are exposed as pin headers)
  // A0, A1 are DAC outputs (ADC2), A2-A4 are ADC2, A5 is ADC1
  pinMode(A0, INPUT_PULLDOWN);
  Serial.printf("Pin A0 set to INPUT_PULLDOWN\n");
  
  pinMode(A1, INPUT_PULLDOWN);
  Serial.printf("Pin A1 set to INPUT_PULLDOWN\n");
  
  pinMode(A2, INPUT_PULLDOWN);
  Serial.printf("Pin A2 set to INPUT_PULLDOWN\n");
  
  pinMode(A3, INPUT_PULLDOWN);
  Serial.printf("Pin A3 set to INPUT_PULLDOWN\n");
  
  pinMode(A4, INPUT_PULLDOWN);
  Serial.printf("Pin A4 set to INPUT_PULLDOWN\n");
  
  pinMode(A5, INPUT_PULLDOWN);
  Serial.printf("Pin A5 set to INPUT_PULLDOWN\n");
  
  // Digital pins 5, 9-13 (these are exposed as pin headers)
  pinMode(5, INPUT_PULLDOWN);
  Serial.printf("Pin 5 set to INPUT_PULLDOWN\n");
  
  pinMode(9, INPUT_PULLDOWN);
  Serial.printf("Pin 9 set to INPUT_PULLDOWN\n");
  
  pinMode(10, INPUT_PULLDOWN);
  Serial.printf("Pin 10 set to INPUT_PULLDOWN\n");
  
  pinMode(11, INPUT_PULLDOWN);
  Serial.printf("Pin 11 set to INPUT_PULLDOWN\n");
  
  pinMode(12, INPUT_PULLDOWN);
  Serial.printf("Pin 12 set to INPUT_PULLDOWN\n");
  
  pinMode(13, INPUT_PULLDOWN);
  Serial.printf("Pin 13 set to INPUT_PULLDOWN\n");
  
  Serial.println("Specific pin headers initialized with pull-down resistors");
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
