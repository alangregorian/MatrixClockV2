#include "../include/wordclock_manager.h"

// Global variables
uint64_t wordMask = 0;
int colorShiftIndex = 0;
Adafruit_NeoMatrix* wordClockMatrix = nullptr;

void initializeWordClock(Adafruit_NeoMatrix& matrix) {
  Serial.println("Initializing WordClock...");
  
  // Store reference to the matrix
  wordClockMatrix = &matrix;
  
  // Don't call matrix.begin() again - it was already called in display_manager
  // Just set our brightness and clear
  matrix.setBrightness(DAYBRIGHTNESS); // Use our defined brightness constant
  matrix.fillScreen(0);
  matrix.show();
  
  // Reset global variables
  wordMask = 0;
  colorShiftIndex = 0;
  
  Serial.println("WordClock initialization complete");
}

void displayWordClockTime(struct tm* timeinfo) {
  if (!wordClockMatrix || !timeinfo) {
    Serial.println("WordClock: Invalid matrix or time info");
    return;
  }
  
  // Clear the mask to ensure we start fresh each time
  clearWordMask();
  
  int hour = timeinfo->tm_hour;
  int minute = timeinfo->tm_min;
  
  // Add 2.5 minutes to get better time estimates (like original)
  minute += 2;
  if (minute >= 60) {
    minute -= 60;
    hour++;
    if (hour >= 24) {
      hour = 0;
    }
  }
  
  // Set minute words based on time
  setMinuteWords(minute);
  
  // Set hour words and PAST/TO based on minute
  if (minute < 5) {
    // Exact hour - just show the hour
    setHourWord(hour);
  } else if (minute < 35) {
    // Past the hour
    addWordToMask(PAST);
    setHourWord(hour);
  } else {
    // To the next hour
    addWordToMask(TO);
    setHourWord((hour + 1) % 24);
  }
  
  // Apply the mask with color effects
  applyWordMask();
}

void setMinuteWords(int minute) {
  if ((minute > 4) && (minute < 10)) {
    addWordToMask(MFIVE);
  } else if ((minute > 9) && (minute < 15)) {
    addWordToMask(MTEN);
  } else if ((minute > 14) && (minute < 20)) {
    addWordToMask(AQUARTER);
  } else if ((minute > 19) && (minute < 25)) {
    addWordToMask(TWENTY);
  } else if ((minute > 24) && (minute < 30)) {
    addWordToMask(TWENTY);
    addWordToMask(MFIVE);
  } else if ((minute > 29) && (minute < 35)) {
    addWordToMask(HALF);
  } else if ((minute > 34) && (minute < 40)) {
    addWordToMask(TWENTY);
    addWordToMask(MFIVE);
  } else if ((minute > 39) && (minute < 45)) {
    addWordToMask(TWENTY);
  } else if ((minute > 44) && (minute < 50)) {
    addWordToMask(AQUARTER);
  } else if ((minute > 49) && (minute < 55)) {
    addWordToMask(MTEN);
  } else if (minute > 54) {
    addWordToMask(MFIVE);
  }
}

void setHourWord(int hour) {
  // Convert to 12-hour format
  int hour12 = hour % 12;
  if (hour12 == 0) hour12 = 12;
  
  switch (hour12) {
    case 1:
      addWordToMask(ONE);
      break;
    case 2:
      addWordToMask(TWO);
      break;
    case 3:
      addWordToMask(THREE);
      break;
    case 4:
      addWordToMask(FOUR);
      break;
    case 5:
      addWordToMask(FIVE);
      break;
    case 6:
      addWordToMask(SIX);
      break;
    case 7:
      addWordToMask(SEVEN);
      break;
    case 8:
      addWordToMask(EIGHT);
      break;
    case 9:
      addWordToMask(NINE);
      break;
    case 10:
      addWordToMask(TEN);
      break;
    case 11:
      addWordToMask(ELEVEN);
      break;
    case 12:
      addWordToMask(TWELVE);
      break;
  }
}

void addWordToMask(uint64_t wordMaskValue) {
  wordMask |= wordMaskValue;
}

void clearWordMask() {
  wordMask = 0;
}

void applyWordMask() {
  if (!wordClockMatrix) {
    Serial.println("WordClock: Matrix not initialized");
    return;
  }
  
  // Apply the mask to each pixel
  for (byte i = 0; i < 64; i++) {
    // bitRead is backwards because bitRead reads rightmost digits first
    boolean masker = bitRead(wordMask, 63 - i);
    
    if (masker) {
      // Pixel should be lit with color
      wordClockMatrix->setPixelColor(i, colorWheel(((i * 256 / wordClockMatrix->numPixels()) + colorShiftIndex) & 255));
    } else {
      // Pixel should be off
      wordClockMatrix->setPixelColor(i, 0, 0, 0);
    }
  }
  
  // Show the result
  wordClockMatrix->show();
  
  // Add delay for color shifting effect
  delay(SHIFTDELAY);
  
  // Move the colors forward
  colorShiftIndex++;
  colorShiftIndex = colorShiftIndex % (256 * 5);
  
  // Reset mask for next time
  clearWordMask();
}

uint32_t colorWheel(byte wheelPos) {
  wheelPos = 255 - wheelPos;
  
  if (wheelPos < 85) {
    return wordClockMatrix->Color(255 - wheelPos * 3, 0, wheelPos * 3);
  } else if (wheelPos < 170) {
    wheelPos -= 85;
    return wordClockMatrix->Color(0, wheelPos * 3, 255 - wheelPos * 3);
  } else {
    wheelPos -= 170;
    return wordClockMatrix->Color(wheelPos * 3, 255 - wheelPos * 3, 0);
  }
}

void rainbowCycle(uint8_t wait) {
  if (!wordClockMatrix) {
    Serial.println("WordClock: Matrix not initialized for rainbow cycle");
    return;
  }
  
  uint16_t i, j;
  
  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < wordClockMatrix->numPixels(); i++) {
      wordClockMatrix->setPixelColor(i, colorWheel(((i * 256 / wordClockMatrix->numPixels()) + j) & 255));
    }
    wordClockMatrix->show();
    delay(wait);
  }
}

void flashWords() {
  if (!wordClockMatrix) {
    Serial.println("WordClock: Matrix not initialized for flash words");
    return;
  }
  
  Serial.println("WordClock: Starting flash words sequence");
  
  // Flash the hidden signature first
  clearWordMask();
  addWordToMask(ANDYDORO);
  applyWordMask();
  delay(FLASHDELAY * 2);
  
  // Flash each minute word
  clearWordMask();
  addWordToMask(MFIVE);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(MTEN);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(AQUARTER);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(TWENTY);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(HALF);
  applyWordMask();
  delay(FLASHDELAY);
  
  // Flash connector words
  clearWordMask();
  addWordToMask(TO);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(PAST);
  applyWordMask();
  delay(FLASHDELAY);
  
  // Flash each hour word
  clearWordMask();
  addWordToMask(ONE);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(TWO);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(THREE);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(FOUR);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(FIVE);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(SIX);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(SEVEN);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(EIGHT);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(NINE);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(TEN);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(ELEVEN);
  applyWordMask();
  delay(FLASHDELAY);
  
  clearWordMask();
  addWordToMask(TWELVE);
  applyWordMask();
  delay(FLASHDELAY);
  
  // Blank for a bit
  clearWordMask();
  applyWordMask();
  delay(FLASHDELAY);
  
  Serial.println("WordClock: Flash words sequence complete");
}

void adjustWordClockBrightness(struct tm* timeinfo) {
  if (!wordClockMatrix || !timeinfo) {
    return;
  }
  
  int hour = timeinfo->tm_hour;
  
  // Change brightness if it's night time
  if (hour < MORNINGCUTOFF || hour > NIGHTCUTOFF) {
    wordClockMatrix->setBrightness(NIGHTBRIGHTNESS);
  } else {
    wordClockMatrix->setBrightness(DAYBRIGHTNESS);
  }
}

void testNeoMatrix(Adafruit_NeoMatrix& matrix) {
  Serial.println("WordClock: Testing NeoMatrix with simple patterns...");
  
  // Test 1: Single red pixel at (0,0)
  Serial.println("WordClock: Test 1 - Red pixel at (0,0)");
  matrix.fillScreen(0);
  matrix.drawPixel(0, 0, matrix.Color(255, 0, 0)); // Red
  matrix.show();
  delay(1000);
  
  // Test 2: Single green pixel at (7,7)
  Serial.println("WordClock: Test 2 - Green pixel at (7,7)");
  matrix.fillScreen(0);
  matrix.drawPixel(7, 7, matrix.Color(0, 255, 0)); // Green
  matrix.show();
  delay(1000);
  
  // Test 3: Blue diagonal line
  Serial.println("WordClock: Test 3 - Blue diagonal line");
  matrix.fillScreen(0);
  for(int i = 0; i < 8; i++) {
    matrix.drawPixel(i, i, matrix.Color(0, 0, 255)); // Blue
  }
  matrix.show();
  delay(1000);
  
  // Test 4: Fill entire matrix with dim white
  Serial.println("WordClock: Test 4 - Dim white fill");
  matrix.fillScreen(matrix.Color(50, 50, 50)); // Dim white
  matrix.show();
  delay(1000);
  
  // Test 5: Clear matrix
  Serial.println("WordClock: Test 5 - Clear matrix");
  matrix.fillScreen(0);
  matrix.show();
  delay(500);
  
  Serial.println("WordClock: NeoMatrix test complete!");
}
