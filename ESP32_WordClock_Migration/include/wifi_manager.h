#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_NeoMatrix.h>

// WiFi scanning variables
extern int networkCount;
extern int currentNetwork;
extern unsigned long lastScan;
extern const unsigned long SCAN_INTERVAL;

// Function declarations
void scanWiFiNetworks(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix);
void displayCurrentNetwork(Adafruit_ST7789& tft, Adafruit_NeoMatrix& matrix);
uint16_t getSignalColor(int32_t rssi);
const char* getEncryptionType(wifi_auth_mode_t encryptionType);
void updateNeoMatrixSignal(Adafruit_NeoMatrix& matrix, int32_t rssi);

// Network navigation functions
void nextNetwork();
void previousNetwork();
int getNetworkCount();

#endif // WIFI_MANAGER_H
