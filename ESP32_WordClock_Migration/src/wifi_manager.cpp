#include "../include/wifi_manager.h"

// WiFi scanning variables
int networkCount = 0;
int currentNetwork = 0;
unsigned long lastScan = 0;
const unsigned long SCAN_INTERVAL = 10000; // Scan every 10 seconds

// Selected network variables
String selectedSSID = "";

void scanWiFiNetworks() {
  Serial.println("Scanning for WiFi networks...");
  
  // Perform WiFi scan
  int totalNetworks = WiFi.scanNetworks();
  lastScan = millis();
  
  // Limit to top networks by signal strength (WiFi.scanNetworks() sorts by RSSI)
  networkCount = min(totalNetworks, MAX_NETWORKS);
  
  Serial.printf("Found %d networks, showing top %d by signal strength\n", totalNetworks, networkCount);
  
  if (networkCount > 0) {
    currentNetwork = 0; // Reset to first network
    
    // Safe output - just show we have networks without the problematic loop
    Serial.printf("Strongest network: %s (%d dBm)\n", 
                  WiFi.SSID(0).c_str(), WiFi.RSSI(0));
    if (networkCount > 1) {
      Serial.printf("Total available for selection: %d networks\n", networkCount);
    }
  } else {
    Serial.println("No WiFi networks found");
  }
}

WiFiNetworkInfo getCurrentNetworkInfo() {
  WiFiNetworkInfo info;
  
  if (networkCount > 0 && currentNetwork >= 0 && currentNetwork < networkCount) {
    info.ssid = WiFi.SSID(currentNetwork);
    info.rssi = WiFi.RSSI(currentNetwork);
    info.encryption = WiFi.encryptionType(currentNetwork);
  } else {
    info.ssid = "";
    info.rssi = -100;
    info.encryption = WIFI_AUTH_OPEN;
  }
  
  return info;
}

uint16_t getSignalStrength(int32_t rssi) {
  // Convert RSSI to percentage (0-100)
  // RSSI typically ranges from -30 (excellent) to -90 (poor)
  if (rssi >= -30) return 100;
  if (rssi <= -90) return 0;
  
  // Linear mapping from -30 to -90 dBm to 100% to 0%
  return (uint16_t)((rssi + 90) * 100 / 60);
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
    // Add bounds checking before accessing WiFi data
    if (currentNetwork >= 0 && currentNetwork < networkCount) {
      Serial.printf("Next network: %d (%s)\n", currentNetwork + 1, WiFi.SSID(currentNetwork).c_str());
    }
  }
}

void selectCurrentNetwork() {
  if (networkCount > 0 && currentNetwork >= 0 && currentNetwork < networkCount) {
    selectedSSID = WiFi.SSID(currentNetwork);
    Serial.printf("Selected network: %s\n", selectedSSID.c_str());
  }
}

bool connectToNetwork(String ssid, String password) {
  Serial.printf("Attempting to connect to: %s\n", ssid.c_str());
  
  // Begin WiFi connection
  WiFi.begin(ssid.c_str(), password.c_str());
  
  // Wait for connection with timeout
  unsigned long startTime = millis();
  const unsigned long timeout = 10000; // 10 second timeout
  
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout) {
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully!");
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    return true;
  } else {
    Serial.println("\nWiFi connection failed!");
    return false;
  }
}

String getSelectedSSID() {
  return selectedSSID;
}

int getNetworkCount() {
  return networkCount;
}

bool hasNetworks() {
  return networkCount > 0;
}
