#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>

// Button event types
enum ButtonEvent {
  NO_BUTTON,
  BUTTON_A_PRESSED,
  BUTTON_B_PRESSED,
  BUTTON_C_PRESSED
};

// Button pins (defined in main.cpp)
extern const int BUTTON_A;
extern const int BUTTON_B;
extern const int BUTTON_C;

// Button state variables
extern bool buttonA_pressed;
extern bool buttonB_pressed;
extern bool buttonC_pressed;
extern unsigned long lastButtonPress;
extern const unsigned long DEBOUNCE_DELAY;

// Function declarations
void initializeButtons();
void initializeUnusedPins();
ButtonEvent handleButtons();

#endif // BUTTON_HANDLER_H
