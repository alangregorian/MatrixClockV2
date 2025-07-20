
/*
  Web client

 This sketch connects to a website (wifitest.adafruit.com/testwifi/index.html)
 using the WiFi module.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and UNO WiFi Rev.2)

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */


//#include <SPI.h>
//#include <WiFiNINA.h>

  #define SPIWIFI       SPI
  #define SPIWIFI_SS    10   // Chip select pin
  #define SPIWIFI_ACK    9   // a.k.a BUSY or READY pin
  #define ESP32_RESETN   8   // Reset pin
  #define ESP32_GPIO0   -1   // Not connected

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
//String months;
//String hours;
//String minutes;
//String seconds;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "AJG 2.4GHz";      // your network SSID (name)
char pass[] = "letmeinplease";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

char server[] = "wifitest.adafruit.com";    // name address for adafruit test
char path[]   = "/testwifi/index.html";

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;



// Simple DST check for US Central Time
boolean isDST(int month, int day) {
  if (month < 3 || month > 11) return false;
  if (month > 3 && month < 11) return true;
  if (month == 3) return day >= 8;  // Simplified: assume 2nd Sunday is day 8-14
  if (month == 11) return day < 7;  // Simplified: assume 1st Sunday is day 1-7
  return false;
}

String get_time() {
  Serial.begin(9600);
  while (!Serial) { ; }

  WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);
  while (WiFi.status() == WL_NO_MODULE) {
    delay(1000);
  }

  do {
    status = WiFi.begin(ssid, pass);
  } while (status != WL_CONNECTED);

  timeClient.begin();
  timeClient.setTimeOffset(0); // Get UTC first
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  
  // Parse date and check DST
  formattedDate = timeClient.getFormattedDate();
  int month = formattedDate.substring(5, 7).toInt();
  int day = formattedDate.substring(8, 10).toInt();
  
  // Set correct timezone offset
  int offset = isDST(month, day) ? -18000 : -21600;
  timeClient.setTimeOffset(offset);
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  formattedDate = timeClient.getFormattedDate();
  return formattedDate;
}
