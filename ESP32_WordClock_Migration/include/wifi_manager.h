#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

// WiFi scanning variables
extern int networkCount;
extern int currentNetwork;
extern unsigned long lastScan;
extern const unsigned long SCAN_INTERVAL;

// WiFi network data structure
struct WiFiNetworkInfo {
  String ssid;
  int32_t rssi;
  wifi_auth_mode_t encryption;
};

// Function declarations
void scanWiFiNetworks();
WiFiNetworkInfo getCurrentNetworkInfo();
uint16_t getSignalStrength(int32_t rssi); // Returns 0-100 percentage
const char* getEncryptionType(wifi_auth_mode_t encryptionType);

// Network navigation functions
void nextNetwork();
void previousNetwork();
int getNetworkCount();
bool hasNetworks();

#endif // WIFI_MANAGER_H
