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

// State Variables
int currentAngle = 90;       // Start at center position
int currentLedIndex = 13;    // Start at middle LED (for display purposes only)
bool lastButtonState = HIGH; // Using INPUT_PULLUP, so HIGH is not pressed

void setup() {
  Serial.begin(9600);
  
  // Initialize servos
  arch_left.attach(5);
  arch_right.attach(6);
  
  // Set initial position
  arch_left.write(currentAngle);
  arch_right.write(180 - currentAngle); // Mirror movement
  
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
    // Move arch 10 degrees
    currentAngle += 10;
    
    // Reset to 0 if we go past 180
    if (currentAngle > 180) {
      currentAngle = 0;
    }
    
    // Update servos
    arch_left.write(currentAngle);
    arch_right.write(180 - currentAngle); // Mirror movement
    
    // Update display
    updateDisplay();
    
    // Signal the LED Arduino to move LEDs
    signalLedArduino();
    
    // Debounce delay
    delay(200);
  }
  
  // Save button state for next iteration
  lastButtonState = buttonState;
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
  lcd.print("Angle: ");
  lcd.print(currentAngle);
  lcd.print(" deg");
  
  // Calculate LED position for display purposes
  currentLedIndex = map(currentAngle, 0, 180, 0, 26);
  
  lcd.setCursor(0, 1);
  lcd.print("LED: ");
  lcd.print(currentLedIndex);
}