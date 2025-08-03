/*
  Simple NeoPixel 8x8 Matrix Test
  This will test if your hardware connections are working
*/

#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// NeoMatrix pin - same as your main code
const int NEOPIN = 6;

// Create matrix object - same configuration as your main code
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, NEOPIN,
                            NEO_MATRIX_TOP  + NEO_MATRIX_LEFT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB         + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("NeoPixel 8x8 Matrix Test Starting...");
  Serial.println("Pin: 6");
  Serial.println("Configuration: 8x8, TOP+LEFT+ROWS+PROGRESSIVE, GRB+800KHz");
  
  // Initialize matrix
  matrix.begin();
  matrix.setBrightness(50); // Medium brightness for testing
  matrix.fillScreen(0); // Clear all pixels
  matrix.show();
  
  Serial.println("Matrix initialized. Starting tests...");
  delay(1000);
  
  // Test 1: Single red pixel at (0,0)
  Serial.println("Test 1: Single red pixel at (0,0)");
  matrix.fillScreen(0);
  matrix.drawPixel(0, 0, matrix.Color(255, 0, 0)); // Red
  matrix.show();
  delay(2000);
  
  // Test 2: Single green pixel at (7,7)
  Serial.println("Test 2: Single green pixel at (7,7)");
  matrix.fillScreen(0);
  matrix.drawPixel(7, 7, matrix.Color(0, 255, 0)); // Green
  matrix.show();
  delay(2000);
  
  // Test 3: Blue diagonal line
  Serial.println("Test 3: Blue diagonal line");
  matrix.fillScreen(0);
  for(int i = 0; i < 8; i++) {
    matrix.drawPixel(i, i, matrix.Color(0, 0, 255)); // Blue
  }
  matrix.show();
  delay(2000);
  
  // Test 4: Fill entire matrix with dim white
  Serial.println("Test 4: Fill entire matrix with dim white");
  matrix.fillScreen(matrix.Color(50, 50, 50)); // Dim white
  matrix.show();
  delay(2000);
  
  // Test 5: Clear matrix
  Serial.println("Test 5: Clear matrix");
  matrix.fillScreen(0);
  matrix.show();
  delay(1000);
  
  Serial.println("Basic tests complete. Starting color cycle...");
}

void loop() {
  // Continuous color cycling test
  static int hue = 0;
  
  // Fill matrix with cycling color
  uint32_t color = matrix.ColorHSV(hue * 65536L / 360);
  matrix.fillScreen(color);
  matrix.show();
  
  hue = (hue + 1) % 360;
  delay(50);
  
  // Print status every 100 cycles
  static int counter = 0;
  if (counter % 100 == 0) {
    Serial.print("Color cycling... Hue: ");
    Serial.println(hue);
  }
  counter++;
}
