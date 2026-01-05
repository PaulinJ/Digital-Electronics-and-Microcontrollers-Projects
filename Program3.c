#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 13);

const int SENSOR_PIN = A1;
const int BACKLIGHT_PWM_PIN = 10;   // PWM pin controlling transistor base
const int BRIGHT_POT = A0;          // Potentiometer for brightness input
const int BUZZER_PIN = 6;
const int BUTTON_PIN = 2;
const int FAULT_LED = 9;

const int SAMPLE_SIZE = 10;                  
const unsigned long INACTIVITY_TIMEOUT = 20000; // after 20s to dim LCD
const unsigned long FAULT_DELAY = 10000;        // after 10s for Fault alarm
const unsigned long DISPLAY_UPDATE_DELAY = 200; 
const unsigned int FAULT_THRESHOLD = 50;       

unsigned int sensorReadings[SAMPLE_SIZE];
int readingIndex = 0;
unsigned long sensorSum = 0;
bool pumpFault = false;
unsigned long faultStartTime = 0;
unsigned long lastActivityMillis = 0;
unsigned long lastDisplayUpdate = 0;
int readingsCount = 0; // counts readings until 10

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BACKLIGHT_PWM_PIN, OUTPUT);
  pinMode(FAULT_LED, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Pump Monitor");
  Serial.begin(9600);

  // Initialize buffer with initial reading
  unsigned int initialReading = analogRead(SENSOR_PIN);
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    sensorReadings[i] = initialReading;
  }
  sensorSum = (unsigned long)initialReading * SAMPLE_SIZE;

  delay(1000);
  lcd.clear();
  lastDisplayUpdate = millis();
  lastActivityMillis = millis();
}

void loop() {
  readSensor();
  updateBacklight();

  // Update LCD display
  if (millis() - lastDisplayUpdate >= DISPLAY_UPDATE_DELAY) {
    updateLCD();
    lastDisplayUpdate = millis();
  }

  // Manual reset
  if (digitalRead(BUTTON_PIN) == LOW) {
    resetPump();
  }
}

// ---------------- SENSOR ----------------
void readSensor() {
  unsigned int currentReading = analogRead(SENSOR_PIN);

  // Update moving average buffer
  sensorSum = sensorSum - sensorReadings[readingIndex] + currentReading;
  sensorReadings[readingIndex] = currentReading;
  readingIndex = (readingIndex + 1) % SAMPLE_SIZE;

  readingsCount++;

  // Print moving average and dynamic threshold every 10 readings
  if (readingsCount >= SAMPLE_SIZE) {
    unsigned int avgReading = sensorSum / SAMPLE_SIZE;
    unsigned int dynamicThreshold = (avgReading * 70UL) / 100UL;

    Serial.println(F("\n--- 10-Reading Summary ---"));
    Serial.print(F("Moving Average: "));
    Serial.print(avgReading);
    Serial.print(F("  | Dynamic Threshold: "));
    Serial.println(dynamicThreshold);

    readingsCount = 0;
  }

  // Check pump fault condition
  checkPumpFault();

  // Reset inactivity timer if sensor or pot changes
  static unsigned int lastAvg = 0;
  unsigned int avg = sensorSum / SAMPLE_SIZE;
  int potNow = analogRead(BRIGHT_POT);
  static int lastPot = potNow;

  if (abs((int)avg - (int)lastAvg) > 2 || abs(potNow - lastPot) > 4)
    lastActivityMillis = millis();

  lastAvg = avg;
  lastPot = potNow;
}

// ---------------- PUMP FAULT CHECK ----------------
void checkPumpFault() {
  unsigned int avgReadingNow = sensorSum / SAMPLE_SIZE;

  if (avgReadingNow <= FAULT_THRESHOLD) {
    if (faultStartTime == 0)
      faultStartTime = millis();
    else if ((millis() - faultStartTime) >= FAULT_DELAY) {
      if (!pumpFault) {
        pumpFault = true;
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(FAULT_LED, HIGH);
      }
    }
  } else {
    faultStartTime = 0;
    if (pumpFault) {
      pumpFault = false;
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(FAULT_LED, LOW);
    }
  }
}

// ---------------- BACKLIGHT ----------------
void updateBacklight() {
  int potValue = analogRead(BRIGHT_POT);
  int pwmValue = map(potValue, 0, 1023, 50, 255); // visible range

  // Dim after inactivity
  if (millis() - lastActivityMillis > INACTIVITY_TIMEOUT) {
    pwmValue = pwmValue / 8; // dim LCD after 20s inactivity
  }

  analogWrite(BACKLIGHT_PWM_PIN, pwmValue);
}

// ---------------- LCD ----------------
void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Pump: ");
  lcd.print(pumpFault ? "FAULT " : "OK    ");

  unsigned int avgReading = sensorSum / SAMPLE_SIZE;
  int thrPercent = (avgReading * 100UL) / 1023UL;

  lcd.setCursor(0, 1);
  lcd.print("IN:");
  lcd.setCursor(3, 1);
  if (avgReading < 10) lcd.print("   "); 
  else if (avgReading < 100) lcd.print("  ");
  else if (avgReading < 1000) lcd.print(" ");
  lcd.print(avgReading);

  lcd.print(" Thr:%");
  if (thrPercent < 10) lcd.print("  ");
  else if (thrPercent < 100) lcd.print(" ");
  lcd.print(thrPercent);
}

// ---------------- MANUAL RESET ----------------
void resetPump() {
  pumpFault = false;
  faultStartTime = 0;
  lastActivityMillis = millis();
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(FAULT_LED, LOW);
  lcd.clear();
  lcd.print("Manual reset");
  delay(500);
  lcd.clear();
  lastDisplayUpdate = millis();
}
