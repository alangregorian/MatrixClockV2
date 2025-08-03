#ifndef WORDCLOCK_MANAGER_H
#define WORDCLOCK_MANAGER_H

#include <Arduino.h>
#include <Adafruit_NeoMatrix.h>
#include <time.h>

// WordClock configuration
#define NEOPIN 6  // NeoMatrix connected to pin 6

// Brightness settings
#define DAYBRIGHTNESS 40
#define NIGHTBRIGHTNESS 20

// Time cutoffs for brightness adjustment
#define MORNINGCUTOFF 7   // 7am - when daybrightness begins
#define NIGHTCUTOFF   22  // 10pm - when nightbrightness begins

// Delays for effects
#define FLASHDELAY 250    // delay for startup "flashWords" sequence
#define SHIFTDELAY 100    // controls color shifting speed

// Word mask definitions for 8x8 grid
// Grid layout:
// A T W E N T Y D
// Q U A R T E R Y  
// F I V E H A L F
// D P A S T O R O
// F I V E I G H T
// S I X T H R E E
// T W E L E V E N
// F O U R N I N E

// 64-bit masks for each word (hexadecimal values)
#define MFIVE    0xF00000000000ULL        // "FIVE" (minutes)
#define MTEN     0x5800000000000000ULL    // "TEN" (minutes)
#define AQUARTER 0x80FE000000000000ULL    // "A QUARTER"
#define TWENTY   0x7E00000000000000ULL    // "TWENTY"
#define HALF     0xF0000000000ULL         // "HALF"
#define PAST     0x7800000000ULL          // "PAST"
#define TO       0xC00000000ULL           // "TO"
#define ONE      0x43ULL                  // "ONE"
#define TWO      0xC040ULL                // "TWO"
#define THREE    0x1F0000ULL              // "THREE"
#define FOUR     0xF0ULL                  // "FOUR"
#define FIVE     0xF0000000ULL            // "FIVE" (hours)
#define SIX      0xE00000ULL              // "SIX"
#define SEVEN    0x800F00ULL              // "SEVEN"
#define EIGHT    0x1F000000ULL            // "EIGHT"
#define NINE     0xFULL                   // "NINE"
#define TEN      0x1010100ULL             // "TEN" (hours)
#define ELEVEN   0x3F00ULL                // "ELEVEN"
#define TWELVE   0xFE00ULL                // "TWELVE"
#define ANDYDORO 0x8901008700000000ULL    // Hidden signature

// Global variables
extern uint64_t wordMask;
extern int colorShiftIndex;
extern Adafruit_NeoMatrix* wordClockMatrix;

// Function declarations
void initializeWordClock(Adafruit_NeoMatrix& matrix);
void displayWordClockTime(struct tm* timeinfo);
void applyWordMask();
uint32_t colorWheel(byte wheelPos);
void rainbowCycle(uint8_t wait);
void flashWords();
void adjustWordClockBrightness(struct tm* timeinfo);
void clearWordMask();
void testNeoMatrix(Adafruit_NeoMatrix& matrix); // Simple test function

// Helper functions for word mask manipulation
void addWordToMask(uint64_t wordMaskValue);
void setHourWord(int hour);
void setMinuteWords(int minute);

#endif // WORDCLOCK_MANAGER_H
