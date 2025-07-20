/*
   WORD CLOCK - 8x8 NeoPixel Desktop Edition
   by Andy Doro

   A word clock using NeoPixel RGB LEDs for a color shift effect.

   Hardware:
   - Trinket Pro 5V (should work with other Arduino-compatibles with minor modifications)
   - DS1307 RTC breakout
   - NeoPixel NeoMatrix 8x8


   Software:

   This code requires the following libraries:

   - RTClib https://github.com/adafruit/RTClib
   - DST_RTC https://github.com/andydoro/DST_RTC
   - Adafruit_GFX https://github.com/adafruit/Adafruit-GFX-Library
   - Adafruit_NeoPixel https://github.com/adafruit/Adafruit_NeoPixel
   - Adafruit_NeoMatrix https://github.com/adafruit/Adafruit_NeoMatrix


   Wiring:
   - Solder DS1307 breakout to Trinket Pro, A2 to GND, A3 to PWR, A4 to SDA, A5 to SCL
   If you leave off / clip the unused SQW pin on the RTC breakout, the breakout can sit right on top of the Trinket Pro
   for a compact design! It'll be difficult to reach the Trinket Pro reset button, but you can activate the bootloader by
   plugging in the USB.
   - Solder NeoMatrix 5V to Trinket 5V, GND to GND, DIN to Trinket Pro pin 8.


   grid pattern

    A T W E N T Y D
    Q U A R T E R Y
    F I V E H A L F
    D P A S T O R O
    F I V E I G H T
    S I X T H R E E
    T W E L E V E N
    F O U R N I N E

*/

// include the library code:
#include <Wire.h>
#include <RTClib.h>
#include <DST_RTC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WifiUdp.h>
#include <Time.h>
//#include <DS1307RTC.h>

// define how to write each of the words

// 64-bit "mask" for each pixel in the matrix- is it on or off?
uint64_t mask;

// define masks for each word. we add them with "bitwise or" to generate a mask for the entire "phrase".
#define MFIVE    mask |= 0xF00000000000        // these are in hexadecimal
#define MTEN     mask |= 0x5800000000000000
#define AQUARTER mask |= 0x80FE000000000000
#define TWENTY   mask |= 0x7E00000000000000
#define HALF     mask |= 0xF0000000000
#define PAST     mask |= 0x7800000000
#define TO       mask |= 0xC00000000
#define ONE      mask |= 0x43
#define TWO      mask |= 0xC040
#define THREE    mask |= 0x1F0000
#define FOUR     mask |= 0xF0
#define FIVE     mask |= 0xF0000000
#define SIX      mask |= 0xE00000
#define SEVEN    mask |= 0x800F00
#define EIGHT    mask |= 0x1F000000
#define NINE     mask |= 0xF
#define TEN      mask |= 0x1010100
#define ELEVEN   mask |= 0x3F00
#define TWELVE   mask |= 0xFE00
#define ANDYDORO mask |= 0x8901008700000000

// define pins
#define NEOPIN 6  // connect to DIN on NeoMatrix 8x8
#define RTCGND A2 // use this as DS1307 breakout ground 
#define RTCPWR A3 // use this as DS1307 breakout power


// brightness based on time of day- could try warmer colors at night?
#define DAYBRIGHTNESS 40
#define NIGHTBRIGHTNESS 20

// cutoff times for day / night brightness. feel free to modify.
#define MORNINGCUTOFF 7  // when does daybrightness begin?   7am
#define NIGHTCUTOFF   22 // when does nightbrightness begin? 10pm


// define delays
#define FLASHDELAY 250  // delay for startup "flashWords" sequence
#define SHIFTDELAY 100   // controls color shifting speed

// Configure the pins used for the ESP32 connection
  #define SPIWIFI       SPI  // The SPI port
  #define SPIWIFI_SS     10   // Chip select pin
  #define ESP32_RESETN   8   // Reset pin
  #define SPIWIFI_ACK    9   // a.k.a BUSY or READY pin
  #define ESP32_GPIO0   -1
  

RTC_DS1307 RTC; // Establish clock object
DST_RTC dst_rtc; // DST object

String web_time;
const char* date;
const char* _time;
DateTime theTime; // Holds current clock time

int j;   // an integer for the color shifting effect
bool midnightRecalibrated = false; // Flag to ensure midnight recalibration happens only once per day

// Do you live in a country or territory that observes Daylight Saving Time?
// https://en.wikipedia.org/wiki/Daylight_saving_time_by_country
// Use 1 if you observe DST, 0 if you don't. This is programmed for DST in the US / Canada. If your territory's DST operates differently,
// you'll need to modify the code in the calcTheTime() function to make this work properly.
//#define OBSERVE_DST 1


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//Adafruit_NeoPixel matrix = Adafruit_NeoPixel(64, NEOPIN, NEO_GRB + NEO_KHZ800);

// configure for 8x8 neopixel matrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, NEOPIN,
                            NEO_MATRIX_TOP  + NEO_MATRIX_LEFT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB         + NEO_KHZ800);



void setup() {
  // put your setup code here, to run once:

  //Serial for debugging
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  //CLOCK SECTION------------------------------------------------------
  // set pinmodes
  pinMode(NEOPIN, OUTPUT);

  // set analog pins to power DS1307 breakout!
  pinMode(RTCGND, OUTPUT); // analog 2
  pinMode(RTCPWR, OUTPUT); // analog 3

  // set them going!
  digitalWrite(RTCGND, LOW);  // GND for RTC
  digitalWrite(RTCPWR, HIGH); // PWR for RTC

  // start clock
  Wire.begin();  // Begin I2C
  RTC.begin();   // begin clock
  
  
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  } 
  web_time = get_time();
  // Set RTC directly to NTP time (NTP already handles DST for Chicago timezone)
  RTC.adjust(DateTime(get_year(web_time), get_month(web_time), get_day(web_time), get_hour(web_time), get_minute(web_time), get_second(web_time)));
  
  

  matrix.begin();
  matrix.setBrightness(DAYBRIGHTNESS);
  matrix.fillScreen(0); // Initialize all pixels to 'off'
  matrix.show();

  // startup sequence... do colorwipe?
  // delay(500);
  rainbowCycle(5);
  //delay(500);
  flashWords(); // briefly flash each word in sequence
  //delay(100);
}

void loop() {
  //get current time for recalibration  
  uint8_t hour = RTC.now().hour();
  uint8_t minute = RTC.now().minute();
  uint8_t second = RTC.now().second();
  //Serial.println(hour);
  //Serial.println(minute);
  //Serial.println(second);
  delay(1000);
  
  // Reset the midnight recalibration flag when we're past midnight
  if ((hour != 0) || (minute != 0)) {
    midnightRecalibrated = false;
  }
  
  //Recalibrate at this time of day using NTP (which handles DST automatically)
  if ((hour == 0) && (minute == 0) && (second <= 10) && !midnightRecalibrated) { //set to recalibrate everyday at midnight
    web_time = get_time();
    // Set RTC directly to NTP time (NTP already handles DST for Chicago timezone)
    RTC.adjust(DateTime(get_year(web_time), get_month(web_time), get_day(web_time), get_hour(web_time), get_minute(web_time), get_second(web_time)));
    rainbowCycle(5);
    flashWords();
    midnightRecalibrated = true; // Set flag to prevent multiple recalibrations
  } else if ((minute == 0) && (second <= 5)) {
    rainbowCycle(5);
    rainbowCycle(4);
    rainbowCycle(3);
    rainbowCycle(2);
    rainbowCycle(1);
    rainbowCycle(0.5);
    rainbowCycle(0.1);
    flashWords();
  }
  
  // get the time directly from RTC (NTP already handled DST)
  theTime = RTC.now();
  // add 2.5 minutes to get better estimates
  theTime = DateTime(theTime.unixtime() + 150);

  adjustBrightness();
  displayTime();

  //mode_moon(); // uncomment to show moon mode instead!
}

uint16_t get_year(String web_time) {
  String year = web_time.substring(0, 4);

  uint16_t output = year.toInt();
  return output;
  
}

uint8_t get_month(String web_time) {
  String month = web_time.substring(5, 7);
 
  if (month.startsWith("0")) {
    month = month.substring(1);
  }

  uint8_t output = month.toInt();
  return output;
}

uint8_t get_day(String web_time) {
  String day = web_time.substring(8, 10);
  if (day.startsWith("0")) {
    day = day.substring(1);
  }

  uint8_t output = day.toInt();
  return output;
}

uint8_t get_hour(String web_time) {
  String hours = web_time.substring(11, 13);

  uint8_t output = hours.toInt();
  return output;
}

uint8_t get_minute(String web_time) {
  String minutes = web_time.substring(14, 16);

  uint8_t output = minutes.toInt();
  return output;
}

uint8_t get_second(String web_time) {
  String seconds = web_time.substring(17, 19);

  uint8_t output = seconds.toInt();
  return output;
}
