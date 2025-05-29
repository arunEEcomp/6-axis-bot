#include <Servo.h>

// Create Servo objects
Servo servo1, servo2, servo3, servo4;

// Define pins
const int joyXPin = A0;
const int joyYPin = A1;
const int servo1Pin = 9;
const int servo2Pin = 10;
const int servo3Pin = 11;
const int servo4Pin = 12;
const int buttonToZero = 3;     // New push button
const int buttonTo90 = 2;       // Original push button

// Smoothing parameters
const int smoothingDelay = 15;
const int stepSize = 1;

// Deadband
const int deadband = 50;

// Initialization flags
bool zeroed = false;
bool initialized = false;
bool promptShown = false;

void setup() {
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);
  servo4.attach(servo4Pin);

  pinMode(buttonToZero, INPUT_PULLUP);
  pinMode(buttonTo90, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("System Ready.");
}

void loop() {
  // Show prompt once
  if (!zeroed && !promptShown) {
    Serial.println("➡ Press button on pin 3 to move servos to 0 degrees.");
    promptShown = true;
  }

  // Step 1: Move to 0°
  if (!zeroed && digitalRead(buttonToZero) == LOW) {
    Serial.println("Moving servos to 0 degrees...");
    smoothMoveServo(servo1, 0);
    smoothMoveServo(servo2, 0);
    smoothMoveServo(servo3, 0);
    smoothMoveServo(servo4, 0);
    zeroed = true;
    delay(1000);
    Serial.println("✅ Servos set to 0 degrees.");
    Serial.println("➡ Now press button on pin 2 to move to 90 degrees.");
    return;
  }

  // Step 2: Move to 90° after zeroing
  if (zeroed && !initialized && digitalRead(buttonTo90) == LOW) {
    Serial.println("Initializing servos to 90 degrees...");
    smoothMoveServo(servo1, 90);
    smoothMoveServo(servo2, 90);
    smoothMoveServo(servo3, 90);
    smoothMoveServo(servo4, 90);
    initialized = true;
    delay(1000);
    Serial.println("✅ Initialization complete.");
    Serial.println("🎮 Joystick control now active.");
    return;
  }

  // Step 3: Joystick control
  if (initialized) {
    int joyXValue = analogRead(joyXPin);
    int joyYValue = analogRead(joyYPin);

    if (abs(joyXValue - 512) < deadband) joyXValue = 512;
    if (abs(joyYValue - 512) < deadband) joyYValue = 512;

    int targetServo1Angle = map(joyXValue, 0, 1023, 0, 180);
    int targetServo2Angle = map(joyYValue, 0, 1023, 30, 90);
    int targetServo3Angle = map(targetServo1Angle, 0, 180, 10, 90);
    int targetServo4Angle = targetServo2Angle;

    smoothMoveServo(servo1, targetServo1Angle);
    smoothMoveServo(servo2, targetServo2Angle);
    smoothMoveServo(servo3, targetServo3Angle);
    smoothMoveServo(servo4, targetServo4Angle);

    Serial.print("Joystick X: "); Serial.print(joyXValue);
    Serial.print(" | Servo 1: "); Serial.println(targetServo1Angle);

    Serial.print("Joystick Y: "); Serial.print(joyYValue);
    Serial.print(" | Servo 2: "); Serial.println(targetServo2Angle);

    Serial.print("Servo 3 (from Servo 1): "); Serial.println(targetServo3Angle);
    Serial.print("Servo 4 (from Servo 2): "); Serial.println(targetServo4Angle);

    delay(smoothingDelay);
  }
}

// Smooth movement function
void smoothMoveServo(Servo &servo, int targetAngle) {
  int currentAngle = servo.read();
  if (currentAngle < targetAngle) {
    for (int angle = currentAngle; angle <= targetAngle; angle += stepSize) {
      servo.write(angle);
      delay(smoothingDelay);
    }
  } else {
    for (int angle = currentAngle; angle >= targetAngle; angle -= stepSize) {
      servo.write(angle);
      delay(smoothingDelay);
    }
  }
}
