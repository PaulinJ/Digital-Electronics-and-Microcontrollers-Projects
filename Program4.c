#include <EEPROM.h>
#include <avr/wdt.h>
#include <LiquidCrystal.h>       

LiquidCrystal lcd(12, 11, 5, 4, 3, 6); // RS,E,D4,D5,D6,D7

const int sensorPin = A0;
const int powerPin = A1;
const int ledRedPin = 13;
const int ledGreenPin = 8; 
const int resetButton = 2;
//timing of our watchdog
const unsigned long timeout = 2000;
const float POWER_THRESHOLD = 3.0;
//address where we will save the last sensor data
const int EEPROM_ADDR = 0;

unsigned long lastSerialTime = 0;
const unsigned long SERIAL_INTERVAL = 1500;

const unsigned long SLOW_BLINK = 800;
const unsigned long FAST_BLINK = 150;

bool powerLow = false;
int lastSensorValue = -1;
int lastSavedValue = 0;
//for LED blinking
unsigned long lastBlinkToggle = 0;
bool ledState = false;
unsigned long currentBlinkInterval = SLOW_BLINK;

void setup() {
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(resetButton, INPUT_PULLUP);
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.print("System Booting");
  delay(1000);
  lcd.clear();

  Serial.println();
  Serial.println("=== System Booting ===");
  delay(300);
  //enable watchdog
  wdt_disable();
  delay(10);
  wdt_enable(WDTO_4S); // 4s watchdog
  Serial.println("Watchdog Enabled.");
  //read last saved value
  lastSavedValue = EEPROM.read(EEPROM_ADDR);
  Serial.print("Recovered saved sensor value: ");
  Serial.println(lastSavedValue);

  lcd.setCursor(0, 0);
  lcd.print("Saved Value:");
  lcd.setCursor(0, 1);
  lcd.print(lastSavedValue);
  //initial LED setup
  currentBlinkInterval = SLOW_BLINK;
  lastBlinkToggle = millis();
  ledState = LOW;
  digitalWrite(ledRedPin, ledState);
  
  //Turn ON green LED at startup
  digitalWrite(ledGreenPin, HIGH);

  delay(1000);
  lcd.clear();
}

void loop() {
  int sensorRaw = analogRead(sensorPin);
  int sensorValue = sensorRaw / 4; //we reduce the range of outputs
  float voltage = analogRead(powerPin) * (5.0 / 1023.0);

  if (digitalRead(resetButton) == LOW) {
    delay(30); //a short debounce
    if (digitalRead(resetButton) == LOW) {
      Serial.println();
      Serial.println(">>> Watchdog button pressed. Triggering WDT reset...");

      lcd.clear();
      lcd.print("WDT Reset Btn");
      //blink fast while button is held
      unsigned long pressedStart = millis();
      while (digitalRead(resetButton) == LOW && (millis() - pressedStart) < 2000) {
        blinkHandler(FAST_BLINK);
        if (millis() - lastSerialTime > SERIAL_INTERVAL) {
          Serial.println(" (button held) preparing reset...");
          lastSerialTime = millis();
        }
      }

      Serial.println("Forcing watchdog reset now...");
      Serial.println("Wait...");
      lcd.clear(); lcd.print("Resetting...");
      delay(100);

      wdt_enable(WDTO_15MS);//just a short timeout
      unsigned long waitStart = millis();

      while (1) {
        //wait for watchdog to reset
        if (millis() - waitStart > timeout) {
          Serial.println("Timeout reached! Re-launching system...");
          delay(2000);
          break;
        }
      }
    }
  }
  //check for low power
  if (!powerLow && voltage < POWER_THRESHOLD) {
    powerLow = true;
    lastSavedValue = sensorValue;
    EEPROM.update(EEPROM_ADDR, lastSavedValue);
    Serial.println();
    Serial.println("!!! LOW POWER DETECTED !!!");
    Serial.print("Saved last sensor value to EEPROM: ");
    Serial.println(lastSavedValue);
    Serial.println("System entering low-power hold (normal tasks paused).");
    Serial.println();
    currentBlinkInterval = FAST_BLINK;

    lcd.clear();
    lcd.print("LOW POWER!");
    lcd.setCursor(0, 1);
    lcd.print("Saved: ");
    lcd.print(lastSavedValue);

    //Turn OFF green LED in low-power mode 
    digitalWrite(ledGreenPin, LOW);
  }
  //when in low-power mode
  if (powerLow) {
    blinkHandler(currentBlinkInterval);

    if (millis() - lastSerialTime >= SERIAL_INTERVAL) {
      Serial.print("[LOW POWER HOLD] sensor=");
      Serial.print(lastSavedValue);
      Serial.print("   measured_v=");
      Serial.println(voltage, 2);
      lastSerialTime = millis();

      lcd.clear();
      lcd.print("LOW PWR HOLD");
      lcd.setCursor(0, 1);
      lcd.print("V:");
      lcd.print(voltage, 2);
      lcd.print(" S:");
      lcd.print(lastSavedValue);
    }
    //check if power came back
    if (voltage >= POWER_THRESHOLD) {
      powerLow = false;
      Serial.println();
      Serial.println("+++ POWER RESTORED +++");
      lastSavedValue = EEPROM.read(EEPROM_ADDR);
      Serial.print("Recovered saved value from EEPROM: ");
      Serial.println(lastSavedValue);

      //Turn ON green LED when power restored (normal operation)
      digitalWrite(ledGreenPin, HIGH);

      Serial.println("Resuming normal operations.");
      Serial.println();
      currentBlinkInterval = SLOW_BLINK;
      delay(500);

      lcd.clear();
      lcd.print("POWER RESTORED");
      delay(1000);
      lcd.clear();
    }

    wdt_reset();
    return;
  }
   //normal operation
  blinkHandler(currentBlinkInterval);

  if (millis() - lastSerialTime >= SERIAL_INTERVAL) {
    Serial.print("Sensor: ");
    Serial.print(sensorValue);
    Serial.print(" | Power: ");
    Serial.println(voltage, 2);
    lastSerialTime = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("S:");
    lcd.print(sensorValue);
    lcd.print("  V:");
    lcd.print(voltage, 2);
    lcd.setCursor(0, 1);
    lcd.print("Mode:");
    lcd.print(powerLow ? "LOW " : "NORM");
  }

  lastSensorValue = sensorValue;
  wdt_reset();
  delay(10);
}
//handle LED blinking
void blinkHandler(unsigned long intervalMs) {
  unsigned long now = millis();
  if (now - lastBlinkToggle >= intervalMs) {
    lastBlinkToggle = now;
    ledState = !ledState;
    digitalWrite(ledRedPin, ledState);
  }
}
