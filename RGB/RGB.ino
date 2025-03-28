#include <FastLED.h>

// LED strip configuration
#define NUM_LEDS 27
#define LED_PIN 5
CRGB leds[NUM_LEDS];

// Communication pin from main Arduino
const int led_status_pin = 12;  // Input pin to receive signals

// State variables
int currentLedIndex = 0;
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
  
  // Show initial LED position
  updateLedDisplay(currentLedIndex);
}

void loop() {
  // Read signal from main Arduino
  bool signalState = digitalRead(led_status_pin);
  // for debug print signal change from Uno
  if (signalState != lastSignalState) {
    Serial.print("Signal changed from ");
    Serial.print(lastSignalState ? "HIGH" : "LOW");
    Serial.print(" to ");
    Serial.println(signalState ? "HIGH" : "LOW");
  }
  // Detect rising edge (LOW to HIGH transition)
  if (signalState == HIGH && lastSignalState == LOW) {
    Serial.println("Signal received from main Arduino");
    
    // Clear previous LEDs
    clearAllLeds();
    
    // Increment LED position
    currentLedIndex += 3;  // Move by 3 LEDs for more visible change
    if (currentLedIndex >= NUM_LEDS) {
      currentLedIndex = 0;  // Reset when we reach the end
    }
    
    // Update LED display
    updateLedDisplay(currentLedIndex);
    
    // Small delay to avoid multiple triggers
    delay(50);
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
    leds[centerIndex - 1] = CRGB::Blue;
  }
  
  leds[centerIndex] = CRGB::White;
  
  if (centerIndex < NUM_LEDS - 1) {
    leds[centerIndex + 1] = CRGB::Blue;
  }
  
  FastLED.show();
  Serial.print("LED updated at index: ");
  Serial.println(centerIndex);
}