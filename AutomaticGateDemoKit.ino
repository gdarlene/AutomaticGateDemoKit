#include <Servo.h>

// Pin definitions
const int triggerPin = 2;
const int echoPin = 3;
const int redLedAnodePin = 4;
const int redLedCathodePin = 8;
const int blueLedCathodePin = 7;
const int blueLedPin = 5;
const int servoPin = 6;
const int buzzerPin = 12;

// Constants
const float thresholdDistance = 10.0;     // Distance to detect (cm)
const int closedAngle = 0;
const int openAngle = 95;
const unsigned long closeDelay = 3000;    // 5 seconds after last detection

// Beep settings
const int beepFrequency = 1000;           // Hz
const int beepDuration = 500;             // ms
const int beepInterval = 1000;             // Time between beeps (start to start)
unsigned long lastBeepTime = 0;
bool beepOn = false;

Servo myServo;
bool isGateOpen = false;
unsigned long lastDetectionTime = 0;

void setup() {
  // Pin setup
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLedAnodePin, OUTPUT);
  pinMode(redLedCathodePin, OUTPUT);
  pinMode(blueLedCathodePin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(redLedCathodePin, LOW);    // Cathodes to GND
  digitalWrite(blueLedCathodePin, LOW);

  // Initialize components
  myServo.attach(servoPin);
  myServo.write(closedAngle);             // Gate starts closed
  digitalWrite(redLedAnodePin, HIGH);     // Red LED ON (closed)
  digitalWrite(blueLedPin, LOW);          // Blue LED OFF
  noTone(buzzerPin);                      // Buzzer OFF
}

float getDistance() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration / 58.0;  // Convert to cm
}

void updateBeeping() {
  unsigned long now = millis();

  if (beepOn && (now - lastBeepTime >= beepDuration)) {
    noTone(buzzerPin);
    beepOn = false;
  } else if (!beepOn && (now - lastBeepTime >= beepInterval)) {
    tone(buzzerPin, beepFrequency);
    lastBeepTime = now;
    beepOn = true;
  }
}

void loop() {
  float distance = getDistance();

  if (distance < thresholdDistance) {
    if (!isGateOpen) {
      // Open the gate
      myServo.write(openAngle);
      isGateOpen = true;
      digitalWrite(blueLedPin, HIGH);       // Blue LED ON
      digitalWrite(redLedAnodePin, LOW);    // Red LED OFF
      lastBeepTime = millis();              // Start beeping
      tone(buzzerPin, beepFrequency);
      beepOn = true;
    }
    lastDetectionTime = millis();

    // Keep beeping while gate is open
    updateBeeping();
  } 
  else {
    if (isGateOpen && (millis() - lastDetectionTime > closeDelay)) {
      // Close the gate
      myServo.write(closedAngle);
      isGateOpen = false;
      digitalWrite(redLedAnodePin, HIGH);   // Red LED ON
      digitalWrite(blueLedPin, LOW);        // Blue LED OFF
      noTone(buzzerPin);                    // Stop buzzer
      beepOn = false;
    } else if (isGateOpen) {
      // Continue beeping while still within close delay
      updateBeeping();
    }
  }

  delay(10);  // Short delay for stability
}
