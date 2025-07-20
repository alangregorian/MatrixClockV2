// ESP32-S2 Test Sketch
// This sketch will test basic functionality and identify the ESP32 variant

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== ESP32 Detection Test ===");
  Serial.println();
  
  // Print chip information
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel());
  
  Serial.print("Chip Revision: ");
  Serial.println(ESP.getChipRevision());
  
  Serial.print("CPU Frequency: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  
  Serial.print("Flash Size: ");
  Serial.print(ESP.getFlashChipSize() / (1024 * 1024));
  Serial.println(" MB");
  
  Serial.print("Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  
  Serial.print("SDK Version: ");
  Serial.println(ESP.getSdkVersion());
  
  // Test built-in LED (if available)
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  Serial.println("Built-in LED test starting...");
}

void loop() {
  // Blink built-in LED
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED ON");
  delay(1000);
  
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED OFF");
  delay(1000);
}
