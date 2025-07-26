/*
   ESP32-S2 WiFi Scanner with TFT Display
   Migration Step 1 for WordClock Project
   
   Hardware:
   - Adafruit ESP32-S2 Reverse TFT Feather
   - Built-in 240x135 ST7789 TFT display
   - Built-in WiFi
   - 3 tactile buttons (D0, D1, D2)
   - Optional: 8x8 NeoMatrix connected to pin 6
   
   This sketch scans for WiFi networks and displays them on the TFT screen
   Use buttons to navigate through the list
*/

#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// TFT Display pins for ESP32-S2 Reverse TFT Feather (corrected from pinout diagram)
#define TFT_CS    42  // GPIO42 - Chip Select (was incorrectly 7)
#define TFT_RST   41  // GPIO41 - Reset
#define TFT_DC    40  // GPIO40 - Data/Command
#define TFT_MOSI  35  // GPIO35 - SPI MOSI
#define TFT_SCLK  36  // GPIO36 - SPI SCK
#define TFT_BACKLIGHT 45  // GPIO45 - Backlight control

// Button pins
#define BUTTON_A  0   // D0 (also BOOT button)
#define BUTTON_B  1   // D1
#define BUTTON_C  2   // D2

// NeoMatrix pin (for future WordClock integration)
#define NEOPIN    6

// TFT Display setup
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// NeoMatrix setup (8x8 for WordClock)
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, NEOPIN,
                            NEO_MATRIX_TOP  + NEO_MATRIX_LEFT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB         + NEO_KHZ800);

// WiFi scanning variables
int networkCount = 0;
int currentNetwork = 0;
unsigned long lastScan = 0;
const unsigned long SCAN_INTERVAL = 10000; // Scan every 10 seconds

// Button state tracking
bool buttonA_pressed = false;
bool buttonB_pressed = false;
bool buttonC_pressed = false;
unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_DELAY = 200;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32-S2 WiFi Scanner with TFT Display");
  Serial.println("=====================================");
  
  // Initialize buttons
  pinMode(BUTTON_A, INPUT_PULLUP);  // D0: pulled HIGH, goes LOW when pressed
  pinMode(BUTTON_B, INPUT);         // D1: pulled LOW by hardware, goes HIGH when pressed
  pinMode(BUTTON_C, INPUT);         // D2: pulled LOW by hardware, goes HIGH when pressed
  
  // Initialize TFT backlight
  Serial.println("Initializing TFT backlight...");
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH); // Turn on backlight
  
  // Initialize SPI for TFT display
  Serial.println("Initializing SPI...");
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  
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
  
  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  // Initialize NeoMatrix (for future use)
  matrix.begin();
  matrix.setBrightness(20);
  matrix.fillScreen(0);
  matrix.show();
  
  // Show a simple pattern on NeoMatrix to indicate startup
  for(int i = 0; i < 8; i++) {
    matrix.drawPixel(i, i, matrix.Color(0, 50, 0)); // Green diagonal
  }
  matrix.show();
  
  tft.setCursor(10, 85);
  tft.println("Ready! Scanning...");
  
  delay(2000);
  
  // Perform initial WiFi scan
  scanWiFiNetworks();
}

void loop() {
  // Check for button presses
  handleButtons();
  
  // Removed periodic WiFi scan - only scan on button press or startup
  
  delay(50); // Small delay to prevent excessive CPU usage
}

void scanWiFiNetworks() {
  Serial.println("Scanning for WiFi networks...");
  
  // Clear screen and show scanning message
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Scanning...");
  
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
  } else {
    currentNetwork = 0; // Reset to first network
    displayCurrentNetwork();
  }
}

void displayCurrentNetwork() {
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
  drawSignalBars(rssi, 5, 70);
  
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
  
  
  // Print to serial for debugging
  Serial.printf("Network %d: %s (%d dBm) %s\n", 
                currentNetwork + 1, 
                WiFi.SSID(currentNetwork).c_str(), 
                rssi, 
                getEncryptionType(encryption));
}

void handleButtons() {
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
    return;
  }
  
  // Button A / D0 (Down/Next)
  if (digitalRead(BUTTON_A) == LOW && !buttonA_pressed) {
    buttonA_pressed = true;
    lastButtonPress = currentTime;
    
    if (networkCount > 0) {
      currentNetwork = (currentNetwork + 1) % networkCount;
      displayCurrentNetwork();
      Serial.printf("Button D0: Next network (%d)\n", currentNetwork);
    }
  } else if (digitalRead(BUTTON_A) == HIGH) {
    buttonA_pressed = false;
  }
  
  // Button B / D1 (Up/Previous) - D1 goes HIGH when pressed
  if (digitalRead(BUTTON_B) == HIGH && !buttonB_pressed) {
    buttonB_pressed = true;
    lastButtonPress = currentTime;
    
    if (networkCount > 0) {
      currentNetwork = (currentNetwork - 1 + networkCount) % networkCount;
      displayCurrentNetwork();
      Serial.printf("Button D1: Previous network (%d)\n", currentNetwork);
    }
  } else if (digitalRead(BUTTON_B) == LOW) {
    buttonB_pressed = false;
  }
  
  // Button C / D2 (Rescan) - D2 goes HIGH when pressed
  if (digitalRead(BUTTON_C) == HIGH && !buttonC_pressed) {
    buttonC_pressed = true;
    lastButtonPress = currentTime;
    
    Serial.println("Button D2: Rescanning WiFi networks");
    scanWiFiNetworks();
  } else if (digitalRead(BUTTON_C) == LOW) {
    buttonC_pressed = false;
  }
}

uint16_t getSignalColor(int32_t rssi) {
  if (rssi > -50) return ST77XX_GREEN;      // Excellent
  else if (rssi > -60) return ST77XX_YELLOW; // Good
  else if (rssi > -70) return ST77XX_ORANGE; // Fair
  else return ST77XX_RED;                    // Poor
}

void drawSignalBars(int32_t rssi, int x, int y) {
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
