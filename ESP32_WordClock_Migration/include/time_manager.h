#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "settings_manager.h"

// Forward declaration for NTPClient
class NTPClient;

// Time sync status
enum TimeSyncStatus {
  TIME_NOT_SYNCED,
  TIME_SYNCING,
  TIME_SYNC_SUCCESS,
  TIME_SYNC_FAILED
};

class TimeManager {
private:
  WiFiUDP* ntpUDP;
  NTPClient* timeClient;
  
  // Time configuration
  const char* ntpServer;
  int timezoneOffset;
  unsigned long lastSyncTime;
  unsigned long lastSyncMillis;
  unsigned long syncInterval;
  TimeSyncStatus syncStatus;
  
  // DST calculation functions
  bool isDST(int month, int day, int year);      // US DST rules
  bool isDSTEU(int month, int day, int year);    // EU DST rules
  int calculateTimezoneOffset();
  
  // NTP communication
  bool sendNTPPacket();

public:
  // Constructor
  TimeManager();
  
  // Destructor
  ~TimeManager();
  
  // Initialization
  bool initialize();
  void cleanup();
  
  // Time synchronization
  bool syncTime();
  bool forceSync();
  bool needsTimeSync();
  TimeSyncStatus getSyncStatus();
  unsigned long getLastSyncTime();
  
  // Time retrieval
  String getFormattedTime();        // "HH:MM:SS"
  String getFormattedDate();        // "YYYY-MM-DD"
  String getFormattedDateTime();    // "YYYY-MM-DD HH:MM:SS"
  unsigned long getCurrentEpoch();  // Unix timestamp
  int getHours();
  int getMinutes();
  int getSeconds();
  int getDay();
  int getMonth();
  int getYear();
  
  // Configuration
  void setNTPServer(const char* server);
  void setSyncInterval(unsigned long intervalMs);
  
  // Status
  bool isTimeValid();
  String getStatusString();
};

// Global time manager instance
extern TimeManager timeManager;

#endif // TIME_MANAGER_H
