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
  clearTFTScreen(tft);
  
  // Test display with a simple pattern
  Serial.println("Testing TFT display...");
  tft.fillRect(0, 0, 240, 10, ST77XX_RED);    // Red bar at top
  tft.fillRect(0, 125, 240, 10, ST77XX_BLUE); // Blue bar at bottom
  delay(1000);
  clearTFTScreen(tft);
  
  // Initialize NeoMatrix (matching working test configuration)
  Serial.println("Initializing NeoMatrix...");
  matrix.begin();
  matrix.setBrightness(50); // Use same brightness as working test
  matrix.fillScreen(0); // Clear all pixels
  matrix.show();
  
  // Test the matrix with a comprehensive test pattern
  Serial.println("Testing NeoMatrix with comprehensive test...");
  
  // Test 1: Single pixels
  matrix.drawPixel(0, 0, matrix.Color(255, 0, 0)); // Red pixel at (0,0)
  matrix.show();
  delay(500);
  
  matrix.fillScreen(0);
  matrix.drawPixel(7, 7, matrix.Color(0, 255, 0)); // Green pixel at (7,7)
  matrix.show();
  delay(500);
  
  // Test 2: Diagonal line
  matrix.fillScreen(0);
  for(int i = 0; i < 8; i++) {
    matrix.drawPixel(i, i, matrix.Color(0, 0, 255)); // Blue diagonal
  }
  matrix.show();
  delay(1000);
  
  // Test 3: Brief full matrix test
  matrix.fillScreen(matrix.Color(20, 20, 20)); // Dim white
  matrix.show();
  delay(500);
  
  // Clear and finish
  matrix.fillScreen(0);
  matrix.show();
  
  Serial.println("Display initialization complete");
}

void displayStartupMessage(Adafruit_ST7789& tft) {
  // Clear screen first to remove clock logo
  clearTFTScreen(tft);
  
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
  clearTFTScreen(tft);
  
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

void displayCurrentNetwork(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix, const WiFiNetworkInfo& networkInfo, int currentIndex, int totalNetworks) {
  // Clear screen first to remove previous content
  clearTFTScreen(tft);
  
  // Display header
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("WiFi Networks");
  
  // Draw separator line
  tft.drawLine(10, 35, 230, 35, ST77XX_BLUE);
  
  // Display current network info
  tft.setTextSize(1);
  tft.setCursor(10, 45);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("SSID: ");
  tft.setTextColor(ST77XX_WHITE);
  
  // Safely truncate SSID if too long (max ~25 chars for 240px width)
  char displaySSID[26]; // Fixed size buffer to prevent heap issues
  if (networkInfo.ssid.length() > 25) {
    strncpy(displaySSID, networkInfo.ssid.c_str(), 22);
    displaySSID[22] = '.';
    displaySSID[23] = '.';
    displaySSID[24] = '.';
    displaySSID[25] = '\0';
  } else {
    strncpy(displaySSID, networkInfo.ssid.c_str(), 25);
    displaySSID[25] = '\0';
  }
  tft.println(displaySSID);
  
  // Display signal strength
  tft.setCursor(10, 60);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Signal: ");
  tft.setTextColor(ST77XX_WHITE);
  tft.print(networkInfo.rssi);
  tft.print(" dBm");
  
  // Draw signal bars
  drawSignalBars(tft, networkInfo.rssi, 150, 60);
  
  // Display encryption type
  tft.setCursor(10, 75);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Security: ");
  tft.setTextColor(ST77XX_WHITE);
  
  // Convert encryption type to readable string (using const char* to avoid String operations)
  const char* encType;
  uint16_t encColor;
  switch (networkInfo.encryption) {
    case WIFI_AUTH_OPEN:
      encType = "Open";
      encColor = ST77XX_RED;
      break;
    case WIFI_AUTH_WEP:
      encType = "WEP";
      encColor = ST77XX_YELLOW;
      break;
    case WIFI_AUTH_WPA_PSK:
      encType = "WPA";
      encColor = ST77XX_GREEN;
      break;
    case WIFI_AUTH_WPA2_PSK:
      encType = "WPA2";
      encColor = ST77XX_GREEN;
      break;
    case WIFI_AUTH_WPA_WPA2_PSK:
      encType = "WPA/WPA2";
      encColor = ST77XX_GREEN;
      break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
      encType = "WPA2-ENT";
      encColor = ST77XX_GREEN;
      break;
    default:
      encType = "Unknown";
      encColor = ST77XX_WHITE;
      break;
  }
  tft.setTextColor(encColor);
  tft.println(encType);
  
  // Display navigation instructions
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(1);
  tft.setCursor(10, 100);
  tft.println("A: Next  B: Select  C: Rescan");
  
  // Display network count info using passed parameters
  tft.setCursor(10, 115);
  tft.setTextColor(0x7BEF); // Light gray color (RGB565 format)
  tft.print("Network ");
  tft.print(currentIndex + 1);
  tft.print(" of ");
  tft.println(totalNetworks);
  
  // Skip NeoMatrix operations for now since we're focusing on TFT stability
  // clearNeoMatrix(matrix);
}

// Display clearing functions
void clearTFTScreen(Adafruit_ST7789& tft, uint16_t color) {
  tft.fillScreen(color);
}

void clearNeoMatrix(Adafruit_NeoMatrix& matrix) {
  matrix.fillScreen(0);
  matrix.show();
}

void displayPasswordEntry(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix, const String& ssid, const String& maskedPassword, char currentChar) {
  // Clear screen first
  clearTFTScreen(tft);
  
  // Display header
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Enter Password");
  
  // Draw separator line
  tft.drawLine(10, 35, 230, 35, ST77XX_BLUE);
  
  // Display selected network SSID
  tft.setTextSize(1);
  tft.setCursor(10, 45);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Network: ");
  tft.setTextColor(ST77XX_WHITE);
  
  // Safely truncate SSID if too long (using fixed buffer to prevent heap issues)
  char displaySSID[21]; // Fixed size buffer for max 20 chars + null terminator
  if (ssid.length() > 20) {
    strncpy(displaySSID, ssid.c_str(), 17);
    displaySSID[17] = '.';
    displaySSID[18] = '.';
    displaySSID[19] = '.';
    displaySSID[20] = '\0';
  } else {
    strncpy(displaySSID, ssid.c_str(), 20);
    displaySSID[20] = '\0';
  }
  tft.println(displaySSID);
  
  // Display current password (masked) - safely handle the String
  tft.setCursor(10, 60);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Password: ");
  tft.setTextColor(ST77XX_WHITE);
  
  // Use safe approach for masked password display
  char safePassword[33]; // Max 32 chars + null terminator
  if (maskedPassword.length() > 32) {
    strncpy(safePassword, maskedPassword.c_str(), 32);
    safePassword[32] = '\0';
  } else {
    strncpy(safePassword, maskedPassword.c_str(), 32);
    safePassword[32] = '\0';
  }
  tft.print(safePassword);
  
  // Display current character being selected
  tft.setCursor(10, 75);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Current: ");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.print(currentChar);
  
  // Display instructions
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(1);
  tft.setCursor(10, 100);
  tft.println("A: Cycle Char");
  tft.setCursor(10, 115);
  tft.println("B: Confirm Char  C: Submit");
  
  // Skip NeoMatrix operations for stability
  // clearNeoMatrix(matrix);
}

void displayConnectingMessage(Adafruit_ST7789& tft, const String& ssid) {
  // Clear screen first
  clearTFTScreen(tft);
  
  // Display header
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("WiFi Connection");
  
  // Draw separator line
  tft.drawLine(10, 35, 230, 35, ST77XX_BLUE);
  
  // Display connecting message
  tft.setTextSize(2);
  tft.setCursor(10, 50);
  tft.setTextColor(ST77XX_YELLOW);
  tft.println("Connecting...");
  
  // Display network name
  tft.setTextSize(1);
  tft.setCursor(10, 80);
  tft.setTextColor(ST77XX_CYAN);
  tft.print("Network: ");
  tft.setTextColor(ST77XX_WHITE);
  
  // Safely truncate SSID if too long
  char displaySSID[26]; // Fixed size buffer to prevent heap issues
  if (ssid.length() > 25) {
    strncpy(displaySSID, ssid.c_str(), 22);
    displaySSID[22] = '.';
    displaySSID[23] = '.';
    displaySSID[24] = '.';
    displaySSID[25] = '\0';
  } else {
    strncpy(displaySSID, ssid.c_str(), 25);
    displaySSID[25] = '\0';
  }
  tft.println(displaySSID);
  
  // Display status message
  tft.setCursor(10, 100);
  tft.setTextColor(ST77XX_GREEN);
  tft.println("Please wait...");
  
  // Display timeout info
  tft.setCursor(10, 115);
  tft.setTextColor(0x7BEF); // Light gray color
  tft.println("Timeout: 10 seconds");
}

void displayWiFiSuccess(Adafruit_ST7789& tft, const String& ssid, const String& ipAddress, int32_t rssi) {
  // Clear screen first
  clearTFTScreen(tft);
  
  // Display header with success icon
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("WiFi Connected!");
  
  // Draw separator line
  tft.drawLine(10, 35, 230, 35, ST77XX_GREEN);
  
  // Display network name
  tft.setTextSize(1);
  tft.setCursor(10, 45);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Network: ");
  tft.setTextColor(ST77XX_WHITE);
  
  // Safely truncate SSID if too long
  char displaySSID[21]; // Fixed size buffer to prevent heap issues
  if (ssid.length() > 20) {
    strncpy(displaySSID, ssid.c_str(), 17);
    displaySSID[17] = '.';
    displaySSID[18] = '.';
    displaySSID[19] = '.';
    displaySSID[20] = '\0';
  } else {
    strncpy(displaySSID, ssid.c_str(), 20);
    displaySSID[20] = '\0';
  }
  tft.println(displaySSID);
  
  // Display IP address
  tft.setCursor(10, 60);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("IP Address: ");
  tft.setTextColor(ST77XX_CYAN);
  
  // Safely handle IP address string
  char displayIP[16]; // Max IP length is 15 chars + null terminator
  if (ipAddress.length() > 15) {
    strncpy(displayIP, ipAddress.c_str(), 15);
    displayIP[15] = '\0';
  } else {
    strncpy(displayIP, ipAddress.c_str(), 15);
    displayIP[15] = '\0';
  }
  tft.println(displayIP);
  
  // Display signal strength
  tft.setCursor(10, 75);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Signal: ");
  tft.setTextColor(ST77XX_WHITE);
  tft.print(rssi);
  tft.print(" dBm");
  
  // Draw signal bars
  drawSignalBars(tft, rssi, 150, 75);
  
  // Display success message
  tft.setCursor(10, 95);
  tft.setTextColor(ST77XX_GREEN);
  tft.println("Connection successful!");
  
  // Display instruction
  tft.setCursor(10, 115);
  tft.setTextColor(0x7BEF); // Light gray color
  tft.println("Press any button to continue");
}

void displayWiFiFailure(Adafruit_ST7789& tft, const String& ssid) {
  // Clear screen first
  clearTFTScreen(tft);
  
  // Display header with failure indication
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Connection Failed");
  
  // Draw separator line
  tft.drawLine(10, 35, 230, 35, ST77XX_RED);
  
  // Display network name that failed
  tft.setTextSize(1);
  tft.setCursor(10, 45);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Network: ");
  tft.setTextColor(ST77XX_WHITE);
  
  // Safely truncate SSID if too long
  char displaySSID[21]; // Fixed size buffer to prevent heap issues
  if (ssid.length() > 20) {
    strncpy(displaySSID, ssid.c_str(), 17);
    displaySSID[17] = '.';
    displaySSID[18] = '.';
    displaySSID[19] = '.';
    displaySSID[20] = '\0';
  } else {
    strncpy(displaySSID, ssid.c_str(), 20);
    displaySSID[20] = '\0';
  }
  tft.println(displaySSID);
  
  // Display possible reasons
  tft.setCursor(10, 65);
  tft.setTextColor(ST77XX_YELLOW);
  tft.println("Possible causes:");
  tft.setCursor(10, 80);
  tft.setTextColor(0x7BEF); // Light gray color
  tft.println("- Wrong password");
  tft.setCursor(10, 90);
  tft.println("- Weak signal");
  tft.setCursor(10, 100);
  tft.println("- Network unavailable");
  
  // Display retry options
  tft.setCursor(10, 120);
  tft.setTextColor(ST77XX_CYAN);
  tft.println("A: Retry  B: New Pass  C: Back");
}

void displayTimeSyncStatus(Adafruit_ST7789& tft, const String& status) {
  // Clear screen first
  clearTFTScreen(tft);
  
  // Display header
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Time Sync");
  
  // Draw separator line
  tft.drawLine(10, 35, 230, 35, ST77XX_BLUE);
  
  // Display sync status
  tft.setTextSize(2);
  tft.setCursor(10, 50);
  
  // Color based on status
  if (status == "Syncing...") {
    tft.setTextColor(ST77XX_YELLOW);
  } else if (status == "Synced") {
    tft.setTextColor(ST77XX_GREEN);
  } else if (status == "Sync Failed") {
    tft.setTextColor(ST77XX_RED);
  } else {
    tft.setTextColor(ST77XX_WHITE);
  }
  
  // Safely display status
  char displayStatus[21]; // Fixed size buffer
  if (status.length() > 20) {
    strncpy(displayStatus, status.c_str(), 20);
    displayStatus[20] = '\0';
  } else {
    strncpy(displayStatus, status.c_str(), 20);
    displayStatus[20] = '\0';
  }
  tft.println(displayStatus);
  
  // Display additional info
  tft.setTextSize(1);
  tft.setCursor(10, 85);
  tft.setTextColor(ST77XX_CYAN);
  tft.println("Connecting to NTP server...");
  
  tft.setCursor(10, 100);
  tft.setTextColor(0x7BEF); // Light gray color
  tft.println("Server: pool.ntp.org");
  
  tft.setCursor(10, 115);
  tft.println("Please wait...");
}

void displayCurrentTime(Adafruit_ST7789& tft, const String& timeString, const String& dateString) {
  // Clear screen first
  clearTFTScreen(tft);
  
  // Display large time
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 30);
  
  // Safely display time
  char displayTime[12]; // Fixed size buffer for HH:MM:SS + null
  if (timeString.length() > 11) {
    strncpy(displayTime, timeString.c_str(), 11);
    displayTime[11] = '\0';
  } else {
    strncpy(displayTime, timeString.c_str(), 11);
    displayTime[11] = '\0';
  }
  tft.println(displayTime);
  
  // Display date
  tft.setTextSize(2);
  tft.setCursor(30, 80);
  tft.setTextColor(ST77XX_CYAN);
  
  // Safely display date
  char displayDate[15]; // Fixed size buffer for YYYY-MM-DD + null
  if (dateString.length() > 14) {
    strncpy(displayDate, dateString.c_str(), 14);
    displayDate[14] = '\0';
  } else {
    strncpy(displayDate, dateString.c_str(), 14);
    displayDate[14] = '\0';
  }
  tft.println(displayDate);
  
  // Display timezone info
  tft.setTextSize(1);
  tft.setCursor(10, 115);
  tft.setTextColor(0x7BEF); // Light gray color
  tft.println("Central Time (US)");
}

void displayClockScreen(Adafruit_ST7789& tft, const String& timeString, const String& dateString, const String& status) {
  // Clear screen first
  clearTFTScreen(tft);
  
  // Display header
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(1);
  tft.setCursor(10, 5);
  tft.println("ESP32 WordClock");
  
  // Draw separator line
  tft.drawLine(10, 20, 230, 20, ST77XX_GREEN);
  
  // Display large time
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 35);
  
  // Safely display time
  char displayTime[12]; // Fixed size buffer for HH:MM:SS + null
  if (timeString.length() > 11) {
    strncpy(displayTime, timeString.c_str(), 11);
    displayTime[11] = '\0';
  } else {
    strncpy(displayTime, timeString.c_str(), 11);
    displayTime[11] = '\0';
  }
  tft.println(displayTime);
  
  // Display date
  tft.setTextSize(1);
  tft.setCursor(50, 75);
  tft.setTextColor(ST77XX_CYAN);
  
  // Safely display date
  char displayDate[15]; // Fixed size buffer for YYYY-MM-DD + null
  if (dateString.length() > 14) {
    strncpy(displayDate, dateString.c_str(), 14);
    displayDate[14] = '\0';
  } else {
    strncpy(displayDate, dateString.c_str(), 14);
    displayDate[14] = '\0';
  }
  tft.println(displayDate);
  
  // Display sync status
  tft.setCursor(10, 95);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Status: ");
  
  // Color based on status
  if (status == "Synced") {
    tft.setTextColor(ST77XX_GREEN);
  } else if (status == "Sync Failed") {
    tft.setTextColor(ST77XX_RED);
  } else {
    tft.setTextColor(ST77XX_YELLOW);
  }
  
  // Safely display status
  char displayStatus[16]; // Fixed size buffer
  if (status.length() > 15) {
    strncpy(displayStatus, status.c_str(), 15);
    displayStatus[15] = '\0';
  } else {
    strncpy(displayStatus, status.c_str(), 15);
    displayStatus[15] = '\0';
  }
  tft.println(displayStatus);
  
  // Display timezone info
  tft.setCursor(10, 110);
  tft.setTextColor(0x7BEF); // Light gray color
  tft.println("Central Time (US)");
  
  // Display button instructions
  tft.setCursor(10, 125);
  tft.setTextColor(0x5AEB); // Darker gray
  tft.println("A: Settings  B: Sync  C: WordClock");
}

void displaySettingsMenu(Adafruit_ST7789& tft, int selectedIndex, const String& timezone, const String& dst, const String& brightness, const String& save) {
  // Clear screen first
  clearTFTScreen(tft);
  
  // Display header
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Settings");
  
  // Draw separator line
  tft.drawLine(10, 35, 230, 35, ST77XX_BLUE);
  
  // Settings menu items
  String menuItems[4] = {timezone, dst, brightness, save};
  
  // Display each menu item
  for (int i = 0; i < 4; i++) {
    tft.setCursor(10, 45 + (i * 15));
    tft.setTextSize(1);
    
    // Highlight selected item
    if (i == selectedIndex) {
      tft.setTextColor(ST77XX_BLACK);
      tft.fillRect(8, 43 + (i * 15), 224, 12, ST77XX_YELLOW);
    } else {
      tft.setTextColor(ST77XX_WHITE);
    }
    
    // Safely display menu item
    char displayItem[30]; // Fixed size buffer
    if (menuItems[i].length() > 29) {
      strncpy(displayItem, menuItems[i].c_str(), 29);
      displayItem[29] = '\0';
    } else {
      strncpy(displayItem, menuItems[i].c_str(), 29);
      displayItem[29] = '\0';
    }
    tft.println(displayItem);
  }
  
  // Display instructions
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(1);
  tft.setCursor(10, 115);
  tft.println("A: Navigate  B: Change  C: Cancel");
}

void clearAllDisplays(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix) {
  clearTFTScreen(tft);
  clearNeoMatrix(matrix);
}

// WordClock display functions
void displayWordClockMode(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix, struct tm* timeinfo) {
  if (!timeinfo) {
    Serial.println("Display: Invalid time info for WordClock mode");
    return;
  }
  
  // Clear TFT and show WordClock status
  clearTFTScreen(tft);
  
  // Display WordClock header on TFT
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("WordClock Mode");
  
  // Draw separator line
  tft.drawLine(10, 35, 230, 35, ST77XX_GREEN);
  
  // Display current time on TFT for reference
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.printf("%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  
  // Display date
  tft.setTextSize(1);
  tft.setCursor(30, 80);
  tft.setTextColor(ST77XX_CYAN);
  tft.printf("%04d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
  
  // Display WordClock status
  tft.setCursor(10, 100);
  tft.setTextColor(ST77XX_YELLOW);
  tft.println("Matrix: Active");
  
  // Display instructions
  tft.setCursor(10, 115);
  tft.setTextColor(0x7BEF); // Light gray
  tft.println("A: Settings  B: Sync  C: WiFi");
  
  // Adjust brightness based on time
  adjustWordClockBrightness(timeinfo);
  
  // Display time on WordClock matrix
  displayWordClockTime(timeinfo);
}

void showWordClockStartup(Adafruit_NeoMatrix& matrix) {
  Serial.println("Display: Starting WordClock startup sequence");
  
  // Initialize WordClock
  initializeWordClock(matrix);
  
  // Show rainbow cycle
  rainbowCycle(5);
  
  // Flash all words
  flashWords();
  
  Serial.println("Display: WordClock startup sequence complete");
}
