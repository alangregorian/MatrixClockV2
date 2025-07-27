#include "../include/time_manager.h"

// We need to include the NTPClient library from the available libraries
// For now, we'll create a simple implementation that can be expanded
#include <WiFi.h>
#include <WiFiUdp.h>

// Simple NTP implementation constants
#define NTP_PACKET_SIZE 48
#define NTP_DEFAULT_LOCAL_PORT 1337
#define SEVENZYYEARS 2208988800UL

// Global time manager instance
TimeManager timeManager;

// Constructor
TimeManager::TimeManager() {
  ntpUDP = nullptr;
  timeClient = nullptr;
  ntpServer = "pool.ntp.org";
  timezoneOffset = -21600; // Default to CST (-6 hours)
  lastSyncTime = 0;
  lastSyncMillis = 0; // Track when the sync occurred in millis()
  syncInterval = 3600000; // 1 hour in milliseconds
  syncStatus = TIME_NOT_SYNCED;
}

// Destructor
TimeManager::~TimeManager() {
  cleanup();
}

// Initialize the time manager
bool TimeManager::initialize() {
  Serial.println("[TIME_MANAGER] Initializing...");
  
  // Check if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[TIME_MANAGER] WiFi not connected");
    return false;
  }
  
  // Create UDP instance
  if (!ntpUDP) {
    ntpUDP = new WiFiUDP();
  }
  
  if (!ntpUDP->begin(NTP_DEFAULT_LOCAL_PORT)) {
    Serial.println("[TIME_MANAGER] Failed to start UDP");
    return false;
  }
  
  Serial.println("[TIME_MANAGER] Initialized successfully");
  return true;
}

// Cleanup resources
void TimeManager::cleanup() {
  if (ntpUDP) {
    ntpUDP->stop();
    delete ntpUDP;
    ntpUDP = nullptr;
  }
  
  if (timeClient) {
    delete timeClient;
    timeClient = nullptr;
  }
}

// Simple NTP packet sending
bool TimeManager::sendNTPPacket() {
  if (!ntpUDP) return false;
  
  byte packetBuffer[NTP_PACKET_SIZE];
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  
  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision
  
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  
  // Send packet
  ntpUDP->beginPacket(ntpServer, 123);
  ntpUDP->write(packetBuffer, NTP_PACKET_SIZE);
  return ntpUDP->endPacket();
}

// Synchronize time with NTP server
bool TimeManager::syncTime() {
  if (!ntpUDP) {
    if (!initialize()) {
      syncStatus = TIME_SYNC_FAILED;
      return false;
    }
  }
  
  Serial.printf("[TIME_MANAGER] Syncing time with %s\n", ntpServer);
  syncStatus = TIME_SYNCING;
  
  // Send NTP packet
  if (!sendNTPPacket()) {
    Serial.println("[TIME_MANAGER] Failed to send NTP packet");
    syncStatus = TIME_SYNC_FAILED;
    return false;
  }
  
  // Wait for response
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) { // 5 second timeout
    if (ntpUDP->parsePacket()) {
      byte packetBuffer[NTP_PACKET_SIZE];
      ntpUDP->read(packetBuffer, NTP_PACKET_SIZE);
      
      // Extract timestamp from packet
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      
      // Convert to Unix timestamp (do NOT apply timezone here - keep UTC)
      unsigned long epoch = secsSince1900 - SEVENZYYEARS;
      
      // Store the synchronized time and when it occurred
      lastSyncTime = epoch;  // Store UTC time
      lastSyncMillis = millis();  // Store when sync occurred
      syncStatus = TIME_SYNC_SUCCESS;
      
      // Debug output
      Serial.printf("[TIME_MANAGER] UTC Epoch: %lu\n", epoch);
      Serial.printf("[TIME_MANAGER] Sync millis: %lu\n", lastSyncMillis);
      Serial.printf("[TIME_MANAGER] Timezone offset: %d seconds\n", calculateTimezoneOffset());
      Serial.printf("[TIME_MANAGER] Time synced successfully: %s\n", getFormattedDateTime().c_str());
      return true;
    }
    delay(100);
  }
  
  Serial.println("[TIME_MANAGER] NTP sync timeout");
  syncStatus = TIME_SYNC_FAILED;
  return false;
}

// Force immediate sync
bool TimeManager::forceSync() {
  return syncTime();
}

// Check if time sync is needed
bool TimeManager::needsTimeSync() {
  if (syncStatus != TIME_SYNC_SUCCESS) return true;
  
  // Check if enough time has passed since last sync
  unsigned long elapsedMs = millis() - lastSyncMillis;
  return elapsedMs > syncInterval;
}

// Get sync status
TimeSyncStatus TimeManager::getSyncStatus() {
  return syncStatus;
}

// Get last sync time
unsigned long TimeManager::getLastSyncTime() {
  return lastSyncTime;
}

// Calculate current timezone offset including DST
int TimeManager::calculateTimezoneOffset() {
  // Get current date for DST calculation
  time_t rawTime = lastSyncTime;
  struct tm* timeInfo = gmtime(&rawTime);
  
  int month = timeInfo->tm_mon + 1;
  int day = timeInfo->tm_mday;
  int year = timeInfo->tm_year + 1900;
  
  // US Central Time: CST (-6) or CDT (-5)
  if (isDST(month, day, year)) {
    return -18000; // CDT: -5 hours in seconds
  } else {
    return -21600; // CST: -6 hours in seconds
  }
}

// DST calculation for US Central Time
bool TimeManager::isDST(int month, int day, int year) {
  if (month < 3 || month > 11) return false;
  if (month > 3 && month < 11) return true;
  
  // Calculate the second Sunday in March (DST starts)
  if (month == 3) {
    // Find the first Sunday in March
    int firstSunday = 1;
    int dayOfWeek = (1 + 2*month + (3*(month+1))/5 + year + year/4 - year/100 + year/400) % 7;
    firstSunday = 8 - dayOfWeek;
    if (firstSunday > 7) firstSunday -= 7;
    
    // Second Sunday is first Sunday + 7
    int secondSunday = firstSunday + 7;
    return day >= secondSunday;
  }
  
  // Calculate the first Sunday in November (DST ends)
  if (month == 11) {
    // Find the first Sunday in November
    int dayOfWeek = (1 + 2*month + (3*(month+1))/5 + year + year/4 - year/100 + year/400) % 7;
    int firstSunday = 8 - dayOfWeek;
    if (firstSunday > 7) firstSunday -= 7;
    
    return day < firstSunday;
  }
  
  return false;
}

// Get current time as Unix timestamp
unsigned long TimeManager::getCurrentEpoch() {
  if (syncStatus != TIME_SYNC_SUCCESS) return 0;
  
  // Calculate elapsed time since last sync in seconds
  unsigned long elapsedMs = millis() - lastSyncMillis;
  unsigned long elapsedSeconds = elapsedMs / 1000;
  
  // Add elapsed time to the stored UTC time, then apply timezone offset
  unsigned long utcTime = lastSyncTime + elapsedSeconds;
  return utcTime + calculateTimezoneOffset();
}

// Format time as HH:MM:SS
String TimeManager::getFormattedTime() {
  if (syncStatus != TIME_SYNC_SUCCESS) return "--:--:--";
  
  unsigned long epoch = getCurrentEpoch();
  int hours = (epoch % 86400L) / 3600;
  int minutes = (epoch % 3600) / 60;
  int seconds = epoch % 60;
  
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);
  return String(timeStr);
}

// Format date as YYYY-MM-DD
String TimeManager::getFormattedDate() {
  if (syncStatus != TIME_SYNC_SUCCESS) return "----/--/--";
  
  unsigned long epoch = getCurrentEpoch();
  time_t rawTime = epoch;
  struct tm* timeInfo = gmtime(&rawTime);
  
  char dateStr[11];
  sprintf(dateStr, "%04d-%02d-%02d", 
          timeInfo->tm_year + 1900, 
          timeInfo->tm_mon + 1, 
          timeInfo->tm_mday);
  return String(dateStr);
}

// Format date and time
String TimeManager::getFormattedDateTime() {
  return getFormattedDate() + " " + getFormattedTime();
}

// Get individual time components
int TimeManager::getHours() {
  if (syncStatus != TIME_SYNC_SUCCESS) return 0;
  unsigned long epoch = getCurrentEpoch();
  return (epoch % 86400L) / 3600;
}

int TimeManager::getMinutes() {
  if (syncStatus != TIME_SYNC_SUCCESS) return 0;
  unsigned long epoch = getCurrentEpoch();
  return (epoch % 3600) / 60;
}

int TimeManager::getSeconds() {
  if (syncStatus != TIME_SYNC_SUCCESS) return 0;
  unsigned long epoch = getCurrentEpoch();
  return epoch % 60;
}

int TimeManager::getDay() {
  if (syncStatus != TIME_SYNC_SUCCESS) return 0;
  unsigned long epoch = getCurrentEpoch();
  time_t rawTime = epoch;
  struct tm* timeInfo = gmtime(&rawTime);
  return timeInfo->tm_mday;
}

int TimeManager::getMonth() {
  if (syncStatus != TIME_SYNC_SUCCESS) return 0;
  unsigned long epoch = getCurrentEpoch();
  time_t rawTime = epoch;
  struct tm* timeInfo = gmtime(&rawTime);
  return timeInfo->tm_mon + 1;
}

int TimeManager::getYear() {
  if (syncStatus != TIME_SYNC_SUCCESS) return 0;
  unsigned long epoch = getCurrentEpoch();
  time_t rawTime = epoch;
  struct tm* timeInfo = gmtime(&rawTime);
  return timeInfo->tm_year + 1900;
}

// Configuration methods
void TimeManager::setNTPServer(const char* server) {
  ntpServer = server;
}

void TimeManager::setSyncInterval(unsigned long intervalMs) {
  syncInterval = intervalMs;
}

// Status methods
bool TimeManager::isTimeValid() {
  return syncStatus == TIME_SYNC_SUCCESS;
}

String TimeManager::getStatusString() {
  switch (syncStatus) {
    case TIME_NOT_SYNCED: return "Not Synced";
    case TIME_SYNCING: return "Syncing...";
    case TIME_SYNC_SUCCESS: return "Synced";
    case TIME_SYNC_FAILED: return "Sync Failed";
    default: return "Unknown";
  }
}
