const int TEMP_PIN = A0; // Temperature sensor.

// Low (1) and high (2) reference points.
const int RAW_1 = 104;
const float TEMP_1 = 0.0;

const int RAW_2 = 268;
const float TEMP_2 = 80.0;

// Calibration parameters
float slope = (TEMP_2 - TEMP_1) / (RAW_2 - RAW_1);
float intercept = TEMP_1 - (slope * RAW_1);

void setup() {
  Serial.begin(9600);
  Serial.println("Calibration Parameters:");
  Serial.print("Slope: ");
  Serial.println(slope, 3);
  Serial.print("Intercept: ");
  Serial.println(intercept, 3);
}

void loop() {
  int rawValue = analogRead(TEMP_PIN); // Read data from sensor

  float calibratedTemp = calibrateTemperature(rawValue);

  Serial.print("Raw Data: ");
  Serial.print(rawValue);
  Serial.print(" | Calibrated Temperature: ");
  Serial.print(calibratedTemp);
  Serial.println(" C");

  delay(1000); // Wait 500ms before reading again.
}

float calibrateTemperature(int rawValue) {
  // Apply linear eq. y = mx + c
  return (slope * rawValue) + intercept;
}
