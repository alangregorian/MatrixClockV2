#include "../include/display_manager.h"
#include "../include/version.h"
#include <SPI.h>

// TFT Display pins are predefined by ESP32-S2 Reverse TFT Feather board variant
// No extern declarations needed - using board's built-in pin definitions

void initializeDisplay(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix) {
  // Initialize TFT backlight
  Serial.println("Initializing TFT backlight...");
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH); // Turn on backlight
  
  // Initialize SPI for TFT display (use default SPI pins for ESP32-S2)
  Serial.println("Initializing SPI...");
  SPI.begin();
  
  // Initialize TFT display
  Serial.println("Initializing TFT display...");
  tft.init(135, 240); // 135x240 pixels
  tft.setRotation(3); // Landscape mode (240x135)
  
  Serial.println("Clearing TFT screen...");
  tft.fillScreen(ST77XX_BLACK);
  
  // Test display with a simple pattern
  Serial.println("Testing TFT display...");
  tft.fillRect(0, 0, 240, 10, ST77XX_RED);    // Red bar at top
  tft.fillRect(0, 125, 240, 10, ST77XX_BLUE); // Blue bar at bottom
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  
  // Initialize NeoMatrix
  matrix.begin();
  matrix.setBrightness(20);
  matrix.fillScreen(0);
  matrix.show();
  
  Serial.println("Display initialization complete");
}

void displayStartupMessage(Adafruit_ST7789& tft) {
  // Display startup message
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ESP32-S2 WiFi");
  tft.setCursor(10, 35);
  tft.println("Scanner");
  
  tft.setTextSize(1);
  tft.setCursor(10, 70);
  tft.println("Initializing WiFi...");
  
  tft.setCursor(10, 85);
  tft.println("Ready! Scanning...");
}

void displayClockLogo(Adafruit_ST7789& tft) {
  // Clear screen
  tft.fillScreen(ST77XX_BLACK);
  
  // Display title
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 15);
  tft.println("ESP32 WordClock");
  
  // Draw clock face (centered at 120, 67)
  int centerX = 120;
  int centerY = 67;
  int radius = 30;
  
  // Draw outer circle
  tft.drawCircle(centerX, centerY, radius, ST77XX_WHITE);
  tft.drawCircle(centerX, centerY, radius-1, ST77XX_WHITE);
  
  // Draw hour markers (12, 3, 6, 9)
  for (int i = 0; i < 4; i++) {
    float angle = i * PI / 2 - PI / 2; // Start at 12 o'clock
    int x1 = centerX + (radius - 5) * cos(angle);
    int y1 = centerY + (radius - 5) * sin(angle);
    int x2 = centerX + (radius - 2) * cos(angle);
    int y2 = centerY + (radius - 2) * sin(angle);
    tft.drawLine(x1, y1, x2, y2, ST77XX_WHITE);
  }
  
  // Draw clock hands pointing to 12:00
  // Hour hand (shorter, thicker)
  tft.drawLine(centerX, centerY, centerX, centerY - 15, ST77XX_WHITE);
  tft.drawLine(centerX-1, centerY, centerX-1, centerY - 15, ST77XX_WHITE);
  tft.drawLine(centerX+1, centerY, centerX+1, centerY - 15, ST77XX_WHITE);
  
  // Minute hand (longer, thinner)
  tft.drawLine(centerX, centerY, centerX, centerY - 25, ST77XX_WHITE);
  
  // Center dot
  tft.fillCircle(centerX, centerY, 2, ST77XX_WHITE);
  
  // Display version information
  tft.setTextSize(1);
  tft.setCursor(75, 110);
  tft.print("Version ");
  tft.println(SOFTWARE_VERSION);
  
  // Display build date (smaller text)
  tft.setTextSize(1);
  tft.setCursor(60, 125);
  tft.print("Build: ");
  tft.println(BUILD_DATE);
}

void showStartupPattern(Adafruit_NeoMatrix& matrix) {
  // Show a simple pattern on NeoMatrix to indicate startup
  for(int i = 0; i < 8; i++) {
    matrix.drawPixel(i, i, matrix.Color(0, 50, 0)); // Green diagonal
  }
  matrix.show();
}

void drawSignalBars(Adafruit_ST7789& tft, int32_t rssi, int x, int y) {
  // Draw 4 signal strength bars
  int bars = 0;
  if (rssi > -80) bars = 1;
  if (rssi > -70) bars = 2;
  if (rssi > -60) bars = 3;
  if (rssi > -50) bars = 4;
  
  for (int i = 0; i < 4; i++) {
    uint16_t color = (i < bars) ? ST77XX_GREEN : ST77XX_RED;
    int barHeight = (i + 1) * 3;
    tft.fillRect(x + i * 8, y + 12 - barHeight, 6, barHeight, color);
  }
}
