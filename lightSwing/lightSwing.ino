#include <Servo.h>
#include <FastLED.h>
#include <LiquidCrystal_I2C.h>

// LED Configuration
#define NUM_LEDS 27
#define LED_PIN 5
CRGB leds[NUM_LEDS];

// Servo Configuration
Servo arch_left;
Servo arch_right;

// LCD Configuration (optional)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definitions
const int pushButton = 13;

// State Variables
int currentAngle = 90;       // Start at center position
int currentLedIndex = 13;    // Start at middle LED
bool lastButtonState = HIGH; // Using INPUT_PULLUP, so HIGH is not pressed

void setup() {
  Serial.begin(9600);
  
  // Initialize servos
  arch_left.attach(5);
  arch_right.attach(6);
  
  // Set initial position
  arch_left.write(currentAngle);
  arch_right.write(180 - currentAngle); // Mirror movement
  
  // Initialize LEDs
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, NUM_LEDS);
  clearLEDs();
  updateLED();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  updateDisplay();
  
  // Initialize button
  pinMode(pushButton, INPUT_PULLUP);
}

void loop() {
  // Read button state
  bool buttonState = digitalRead(pushButton);
  
  // Check for button press (transition from HIGH to LOW)
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Move arch 10 degrees
    currentAngle += 10;
    
    // Reset to 0 if we go past 180
    if (currentAngle > 180) {
      currentAngle = 0;
    }
    
    // Update servos
    arch_left.write(currentAngle);
    arch_right.write(180 - currentAngle); // Mirror movement
    
    // Update LED position
    clearLEDs();
    currentLedIndex = map(currentAngle, 0, 180, 0, NUM_LEDS - 1);
    updateLED();
    
    // Update display
    updateDisplay();
    
    // Debounce delay
    delay(200);
  }
  
  // Save button state for next iteration
  lastButtonState = buttonState;
}

void clearLEDs() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void updateLED() {
  // Light up current LED and adjacent ones
  if (currentLedIndex > 0) {
    leds[currentLedIndex - 1] = CRGB::Blue;
  }
  
  leds[currentLedIndex] = CRGB::White;
  
  if (currentLedIndex < NUM_LEDS - 1) {
    leds[currentLedIndex + 1] = CRGB::Blue;
  }
  
  FastLED.show();
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Angle: ");
  lcd.print(currentAngle);
  lcd.print(" deg");
}