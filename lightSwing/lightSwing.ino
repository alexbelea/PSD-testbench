#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// Servo Configuration
Servo arch_left;
Servo arch_right;

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definitions
const int pushButton = 13;
const int led_status_pin = 12;  // Signal to secondary Arduino for LED control

// Preset Positions Configuration
// MODIFY THESE VALUES TO CHANGE PRESET POSITIONS
const int NUM_PRESETS = 5;
int presetAngles[NUM_PRESETS] = {
  20,   // Position 1 - angle in degrees
  50,   // Position 2 - angle in degrees
  90,   // Position 3 - angle in degrees (center)
  130,  // Position 4 - angle in degrees
  160   // Position 5 - angle in degrees
};

// State Variables
int currentPresetIndex = 2;  // Start at center position (index 2, which is the 3rd preset)
bool lastButtonState = HIGH; // Using INPUT_PULLUP, so HIGH is not pressed

void setup() {
  Serial.begin(9600);
  
  // Initialize servos
  arch_left.attach(5);
  arch_right.attach(6);
  
  // Set initial position
  moveToPreset(currentPresetIndex);
  
  // Initialize LED communication pin
  pinMode(led_status_pin, OUTPUT);
  digitalWrite(led_status_pin, LOW);
  
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
    // Move to next preset
    currentPresetIndex = (currentPresetIndex + 1) % NUM_PRESETS;
    
    // Update servo position
    moveToPreset(currentPresetIndex);
    
    // Update display
    updateDisplay();
    
    // Signal the LED Arduino to change LEDs
    signalLedArduino();
    
    // Debounce delay
    delay(200);
  }
  
  // Save button state for next iteration
  lastButtonState = buttonState;
}

void moveToPreset(int presetIndex) {
  // do a calibration each time to ensure correct position
  arch_left.write(0);
  arch_right.write(180); // Mirror movement (180 - 0)
  
  // Wait for servos to reach zero position
  delay(5000);  // Adjust delay as needed for your servos

  int angle = presetAngles[presetIndex];
  
  // Update servos
  arch_left.write(angle);
  arch_right.write(180 - angle); // Mirror movement
  
  Serial.print("Moving to preset ");
  Serial.print(presetIndex + 1);
  Serial.print(" (angle: ");
  Serial.print(angle);
  Serial.println(" degrees)");
}

void signalLedArduino() {
  // Trigger the secondary Arduino's LED animation sequence
  digitalWrite(led_status_pin, HIGH);
  delay(50);  // Brief pulse to trigger the other Arduino
  digitalWrite(led_status_pin, LOW);
  
  Serial.println("Sent signal to LED Arduino");
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Position: ");
  lcd.print(currentPresetIndex + 1);  // Display 1-5 instead of 0-4
  
  // Calculate altitude (angle from horizontal)
  int altitude = presetAngles[currentPresetIndex];
  // Azimuth is implied by the position around the arch
  // For display purposes, we'll map angle 0-180 to azimuth 0-180
  int azimuth = presetAngles[currentPresetIndex];
  
  lcd.setCursor(0, 1);
  lcd.print("Alt:");
  lcd.print(altitude);
  lcd.print(" Az:");
  lcd.print(azimuth);
}