#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup — address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// LED pins
const int idleLED = 10; // Green
const int sterilizingLED = 9; // Yellow
const int errorLED = 6; // Red
const int doorLockLED = 11; // White (PWM-safe)

// Button pins
const int doorButton = 2; // Unlock request
const int emergencyStop = 3; // Emergency stop (active LOW)

// Sensor pins
const int TEMP_POT = A0;
const int PRESS_POT = A1;

// System states
enum State { IDLE, STERILIZING, COMPLETE };
State state = IDLE;

// Timer
unsigned long previousMillis = 0;
const unsigned long stateDuration = 6000; // 6 sec per state

// Safety thresholds
const float SAFE_PRESSURE_KPA = 105.0;
const float SAFE_TEMPERATURE_C = 60.0;

// Sensor readings
float temperature = 25.0;
float pressure = 101.0;

// Flags
bool isDoorLocked = false;
bool errorState = false;
bool sterilizingDisplayDone = false;
bool emergencyTriggered = false; // Track if emergency stop was triggered

// Debounce
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

// Emergency stop state tracking
bool lastEmergencyStopState = HIGH; // Assume button not pressed initially

// ----------------------
// Helper: Safe LED control (PWM-safe)
// ----------------------
void setLED(int pin, bool state) {
  if (pin == 6 || pin == 9 || pin == 10 || pin == 11) {
    analogWrite(pin, state ? 90 : 0); // Safe brightness
  } else {
    digitalWrite(pin, state ? HIGH : LOW);
  }
}

// ----------------------
// Door Control Functions
// ----------------------
void lockDoor() {
  setLED(doorLockLED, true);
  isDoorLocked = true;
  lcd.setCursor(0, 1);
  lcd.print("Door: LOCKED ");
  Serial.println("Door LOCKED");
}

bool unlockDoor() {
  if (digitalRead(emergencyStop) == LOW) {
    setLED(doorLockLED, false);
    isDoorLocked = false;
    lcd.setCursor(0, 1);
    lcd.print("EMERGENCY UNLK");
    Serial.println("UNLOCK: Emergency Stop Activated");
    return true;
  }
  if (state == STERILIZING) {
    lcd.setCursor(0, 1);
    lcd.print("BLOCKED STERIL");
    Serial.println("UNLOCK BLOCKED: Sterilizing in progress");
    return false;
  }
  if (temperature > SAFE_TEMPERATURE_C) {
    lcd.setCursor(0, 1);
    lcd.print("TEMP TOO HIGH ");
    Serial.println("UNLOCK BLOCKED: Temperature too high");
    return false;
  }
  if (pressure > SAFE_PRESSURE_KPA) {
    lcd.setCursor(0, 1);
    lcd.print("PRESS TOO HIGH");
    Serial.println("UNLOCK BLOCKED: Pressure too high");
    return false;
  }
  setLED(doorLockLED, false);
  isDoorLocked = false;
  lcd.setCursor(0, 1);
  lcd.print("Door: UNLOCKED");
  Serial.println("Door UNLOCKED");
  return true;
}

// ----------------------
// SETUP
// ----------------------
void setup() {
  pinMode(idleLED, OUTPUT);
  pinMode(sterilizingLED, OUTPUT);
  pinMode(errorLED, OUTPUT);
  pinMode(doorLockLED, OUTPUT);
  pinMode(doorButton, INPUT_PULLUP);
  pinMode(emergencyStop, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  showIdle();
}

// ----------------------
// MAIN LOOP
// ----------------------
void loop() {
  unsigned long currentMillis = millis();
  bool currentEmergencyStopState = digitalRead(emergencyStop);

  // Check for emergency stop press (HIGH to LOW)
  if (currentEmergencyStopState == LOW && lastEmergencyStopState == HIGH && millis() - lastButtonPress > debounceDelay) {
    if (!emergencyTriggered) {
      // First press: trigger emergency
      triggerError();
      unlockDoor();
      emergencyTriggered = true; // Mark emergency as triggered
      lastButtonPress = currentMillis;
    } else if (errorState) {
      // Second press: reset system
      errorState = false; // Clear error state
      emergencyTriggered = false; // Clear emergency trigger
      showIdle(); // Reset to IDLE state
      lastButtonPress = currentMillis;
      Serial.println("System RESET after emergency stop cleared");
    }
  }

  // Update last emergency stop state
  lastEmergencyStopState = currentEmergencyStopState;

  // Normal state transitions
  if (!errorState && currentMillis - previousMillis >= stateDuration) {
    previousMillis = currentMillis;
    switch (state) {
      case IDLE:
        startSterilization();
        break;
      case STERILIZING:
        finishSterilization();
        break;
      case COMPLETE:
        showIdle();
        break;
    }
  }

  // Handle door button
  if (digitalRead(doorButton) == LOW && millis() - lastButtonPress > debounceDelay) {
    unlockDoor();
    lastButtonPress = millis();
  }

  // Update sensor readings
  simulateSensors();

  // Display sensor readings during STERILIZING
  if (state == STERILIZING && sterilizingDisplayDone && !errorState) {
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temperature, 1);
    lcd.print("C ");
    lcd.setCursor(0, 1);
    lcd.print("Press:");
    lcd.print(pressure, 0);
    lcd.print("kPa ");
    Serial.print("Temp: ");
    Serial.print(temperature, 1);
    Serial.print(" °C, Press: ");
    Serial.print(pressure, 0);
    Serial.println(" kPa");
  }
}

// ----------------------
// Sensor Input from Potentiometers
// ----------------------
void simulateSensors() {
  int rawTemp = analogRead(TEMP_POT);
  int rawPress = analogRead(PRESS_POT);
  temperature = rawTemp * (100.0 / 1023.0); // 0–100°C
  pressure = rawPress * (200.0 / 1023.0); // 0–200 kPa
}

// ----------------------
// State Functions
// ----------------------
void showIdle() {
  state = IDLE;
  sterilizingDisplayDone = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System: IDLE");
  lcd.setCursor(0, 1);
  lcd.print("Door: UNLOCKED");
  Serial.println("State: IDLE");
  setLED(idleLED, true);
  setLED(sterilizingLED, false);
  setLED(doorLockLED, false);
  setLED(errorLED, false);
  isDoorLocked = false;
}

void startSterilization() {
  state = STERILIZING;
  sterilizingDisplayDone = false;
  lockDoor();
  setLED(idleLED, false);
  setLED(sterilizingLED, true);
  setLED(errorLED, false);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STERILIZING...");
  lcd.setCursor(0, 1);
  lcd.print("Door: LOCKED");
  Serial.println("State: STERILIZING");
  delay(3000);
  sterilizingDisplayDone = true;
}

void finishSterilization() {
  state = COMPLETE;
  sterilizingDisplayDone = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CYCLE COMPLETE");
  lcd.setCursor(0, 1);
  lcd.print("Door: UNLOCKED");
  Serial.println("State: COMPLETE");
  setLED(sterilizingLED, false);
  setLED(doorLockLED, false);
  setLED(idleLED, true);
  isDoorLocked = false;
}

// ----------------------
// Emergency Handler
// ----------------------
void triggerError() {
  if (errorState) return;
  errorState = true;
  state = COMPLETE;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("!! EMERGENCY !!");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM HALTED");
  Serial.println("!! EMERGENCY STOP !! SYSTEM HALTED");
  setLED(errorLED, true);
  setLED(idleLED, false);
  setLED(sterilizingLED, false);
}
