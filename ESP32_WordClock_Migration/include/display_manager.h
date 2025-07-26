#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_NeoMatrix.h>
#include "wifi_manager.h"

// Function declarations
void initializeDisplay(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix);
void displayStartupMessage(Adafruit_ST7789& tft);
void displayClockLogo(Adafruit_ST7789& tft);
void showStartupPattern(Adafruit_NeoMatrix& matrix);
void drawSignalBars(Adafruit_ST7789& tft, int32_t rssi, int x, int y);
void displayCurrentNetwork(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix, const WiFiNetworkInfo& networkInfo);
void displayPasswordEntry(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix, const String& ssid, const String& maskedPassword, char currentChar);

// Display clearing functions
void clearTFTScreen(Adafruit_ST7789& tft, uint16_t color = ST77XX_BLACK);
void clearNeoMatrix(Adafruit_NeoMatrix& matrix);
void clearAllDisplays(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix);

#endif // DISPLAY_MANAGER_H
