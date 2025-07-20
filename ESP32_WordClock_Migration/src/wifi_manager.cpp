#include "../include/wifi_manager.h"
#include "../include/display_manager.h"

// WiFi scanning variables
int networkCount = 0;
int currentNetwork = 0;
unsigned long lastScan = 0;
const unsigned long SCAN_INTERVAL = 10000; // Scan every 10 seconds

void scanWiFiNetworks(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix) {
  Serial.println("Scanning for WiFi networks...");
  
  // Clear screen and show scanning message
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Scanning...");
  
  // Update NeoMatrix to show scanning - DISABLED (no hardware connected)
  // matrix.fillScreen(0);
  // for(int i = 0; i < 8; i++) {
  //   matrix.drawPixel(i, 0, matrix.Color(50, 50, 0)); // Yellow top row
  // }
  // matrix.show();
  
  // Perform WiFi scan
  networkCount = WiFi.scanNetworks();
  lastScan = millis();
  
  Serial.printf("Found %d networks\n", networkCount);
  
  if (networkCount == 0) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("No WiFi");
    tft.setCursor(10, 35);
    tft.println("Networks");
    tft.setCursor(10, 60);
    tft.println("Found");
    
    // Red pattern on NeoMatrix - DISABLED (no hardware connected)
    // matrix.fillScreen(matrix.Color(50, 0, 0));
    // matrix.show();
  } else {
    currentNetwork = 0; // Reset to first network
  }
}

void displayCurrentNetwork(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix) {
  if (networkCount == 0) return;
  
  // Clear screen
  tft.fillScreen(ST77XX_BLACK);
  
  // Header
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(1);
  tft.setCursor(5, 5);
  tft.printf("WiFi Networks (%d/%d)", currentNetwork + 1, networkCount);
  
  // Network name
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(5, 25);
  String ssid = WiFi.SSID(currentNetwork);
  if (ssid.length() > 12) {
    ssid = ssid.substring(0, 12) + "...";
  }
  tft.println(ssid);
  
  // Signal strength
  int32_t rssi = WiFi.RSSI(currentNetwork);
  tft.setTextColor(getSignalColor(rssi));
  tft.setTextSize(1);
  tft.setCursor(5, 55);
  tft.printf("Signal: %d dBm", rssi);
  
  // Signal strength bar
  drawSignalBars(tft, rssi, 5, 70);
  
  // Encryption type
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(5, 90);
  wifi_auth_mode_t encryption = WiFi.encryptionType(currentNetwork);
  tft.printf("Security: %s", getEncryptionType(encryption));
  
  // Navigation instructions
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(1);
  tft.setCursor(5, 110);
  tft.println("D0:Down D1:Up D2:Rescan");
  
  // Update NeoMatrix with signal strength visualization - DISABLED (no hardware connected)
  // updateNeoMatrixSignal(matrix, rssi);
  
  // Print to serial for debugging
  Serial.printf("Network %d: %s (%d dBm) %s\n", 
                currentNetwork + 1, 
                WiFi.SSID(currentNetwork).c_str(), 
                rssi, 
                getEncryptionType(encryption));
}

uint16_t getSignalColor(int32_t rssi) {
  if (rssi > -50) return ST77XX_GREEN;      // Excellent
  else if (rssi > -60) return ST77XX_YELLOW; // Good
  else if (rssi > -70) return ST77XX_ORANGE; // Fair
  else return ST77XX_RED;                    // Poor
}


void updateNeoMatrixSignal(Adafruit_NeoMatrix& matrix, int32_t rssi) {
  matrix.fillScreen(0);
  
  // Convert RSSI to number of lit pixels (0-8)
  int pixels = 0;
  if (rssi > -80) pixels = 2;
  if (rssi > -70) pixels = 4;
  if (rssi > -60) pixels = 6;
  if (rssi > -50) pixels = 8;
  
  // Light up pixels in a column to show signal strength
  uint32_t color = matrix.Color(0, 50, 0); // Green for good signal
  if (rssi < -70) color = matrix.Color(50, 25, 0); // Orange for fair
  if (rssi < -80) color = matrix.Color(50, 0, 0);  // Red for poor
  
  for (int i = 0; i < pixels; i++) {
    matrix.drawPixel(3, 7 - i, color); // Center column, bottom up
    matrix.drawPixel(4, 7 - i, color); // Make it 2 pixels wide
  }
  
  matrix.show();
}

const char* getEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case WIFI_AUTH_OPEN:
      return "Open";
    case WIFI_AUTH_WEP:
      return "WEP";
    case WIFI_AUTH_WPA_PSK:
      return "WPA";
    case WIFI_AUTH_WPA2_PSK:
      return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:
      return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return "WPA2-ENT";
    case WIFI_AUTH_WPA3_PSK:
      return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:
      return "WPA2/WPA3";
    default:
      return "Unknown";
  }
}

// Network navigation functions
void nextNetwork() {
  if (networkCount > 0) {
    currentNetwork = (currentNetwork + 1) % networkCount;
    Serial.printf("Next network: %d\n", currentNetwork);
  }
}

void previousNetwork() {
  if (networkCount > 0) {
    currentNetwork = (currentNetwork - 1 + networkCount) % networkCount;
    Serial.printf("Previous network: %d\n", currentNetwork);
  }
}

int getNetworkCount() {
  return networkCount;
}
