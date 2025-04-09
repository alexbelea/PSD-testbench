#include <FastLED.h>

// LED strip configuration
#define NUM_LEDS 27
#define LED_PIN 5
CRGB leds[NUM_LEDS];

// Communication pin from main Arduino
const int led_status_pin = 12;  // Input pin to receive signals

// Preset LED Positions Configuration
// MODIFY THESE VALUES TO CHANGE PRESET LED POSITIONS
const int NUM_PRESETS = 5;
int presetLedPositions[NUM_PRESETS] = {
  3,    // Position 1 - center LED index
  9,    // Position 2 - center LED index
  13,   // Position 3 - center LED index (middle)
  17,   // Position 4 - center LED index
  23    // Position 5 - center LED index
};

// State variables
int currentPresetIndex = 2;  // Start at center position (index 2, which is the 3rd preset)
bool lastSignalState = LOW;

void setup() {
  Serial.begin(9600);
  Serial.println("LED Controller Starting...");
 
  // Initialize LED strip
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(150);  // Moderate brightness (0-255)
 
  // Initialize communication pin
  pinMode(led_status_pin, INPUT);
 
  // Clear all LEDs
  clearAllLeds();
  
  // Explicitly set starting position
  currentPresetIndex = 2;  // Middle position (index 2, which is preset 3)
  
  // Add a small delay for stability
  delay(50);
 
  // Show initial LED position
  updateLedDisplay(presetLedPositions[currentPresetIndex]);
  
  // Debug output
  Serial.print("Starting with preset index: ");
  Serial.println(currentPresetIndex);
}

void loop() {
  // Read signal from main Arduino
  bool signalState = digitalRead(led_status_pin);
  
  // For debug print signal change from Uno
  if (signalState != lastSignalState) {
    Serial.print("Signal changed from ");
    Serial.print(lastSignalState ? "HIGH" : "LOW");
    Serial.print(" to ");
    Serial.println(signalState ? "HIGH" : "LOW");
  }
  
  // Quick delay to ensure signal is still high
  delay(5);
  
  // Detect rising edge (LOW to HIGH transition)
  if (signalState == HIGH && lastSignalState == LOW) {
    Serial.println("Signal received from main Arduino");
    
    // Clear previous LEDs
    clearAllLeds();
    
    // Move to next preset
    currentPresetIndex = (currentPresetIndex + 1) % NUM_PRESETS;
    
    // Update LED display
    updateLedDisplay(presetLedPositions[currentPresetIndex]);
    
    // Small delay to avoid multiple triggers
    delay(5);
  }
  
  // Save current state for next comparison
  lastSignalState = signalState;
}

void clearAllLeds() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void updateLedDisplay(int centerIndex) {
  // Create a 3-LED pattern with center being white and sides being blue
  if (centerIndex > 0) {
    leds[centerIndex - 1] = CRGB::White;
  }
  
  leds[centerIndex] = CRGB::White;
  
  if (centerIndex < NUM_LEDS - 1) {
    leds[centerIndex + 1] = CRGB::White;
  }
  
  FastLED.show();
  Serial.print("LED updated at preset index: ");
  Serial.print(currentPresetIndex + 1);  // Display 1-5 instead of 0-4
  Serial.print(" (LED position: ");
  Serial.print(centerIndex);
  Serial.println(")");
}