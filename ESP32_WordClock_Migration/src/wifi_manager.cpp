#include "../include/wifi_manager.h"

// WiFi scanning variables
int networkCount = 0;
int currentNetwork = 0;
unsigned long lastScan = 0;
const unsigned long SCAN_INTERVAL = 10000; // Scan every 10 seconds

void scanWiFiNetworks() {
  Serial.println("Scanning for WiFi networks...");
  
  // Perform WiFi scan
  networkCount = WiFi.scanNetworks();
  lastScan = millis();
  
  Serial.printf("Found %d networks\n", networkCount);
  
  if (networkCount > 0) {
    currentNetwork = 0; // Reset to first network
    
    // Print all found networks to serial for debugging
    for (int i = 0; i < networkCount; i++) {
      Serial.printf("Network %d: %s (%d dBm) %s\n", 
                    i + 1, 
                    WiFi.SSID(i).c_str(), 
                    WiFi.RSSI(i), 
                    getEncryptionType(WiFi.encryptionType(i)));
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
    Serial.printf("Next network: %d (%s)\n", currentNetwork + 1, WiFi.SSID(currentNetwork).c_str());
  }
}

void previousNetwork() {
  if (networkCount > 0) {
    currentNetwork = (currentNetwork - 1 + networkCount) % networkCount;
    Serial.printf("Previous network: %d (%s)\n", currentNetwork + 1, WiFi.SSID(currentNetwork).c_str());
  }
}

int getNetworkCount() {
  return networkCount;
}

bool hasNetworks() {
  return networkCount > 0;
}
