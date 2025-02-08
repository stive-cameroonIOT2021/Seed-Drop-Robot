#include <AFMotor.h>

// Define motor objects explicitly
AF_DCMotor motorBackRight(1);   // M1 - Back Right
AF_DCMotor motorFrontRight(2);  // M2 - Front Right
AF_DCMotor motorBackLeft(3);    // M3 - Back Left
AF_DCMotor motorFrontLeft(4);   // M4 - Front Left

// Boolean flags to reverse motor directions if needed
bool is_reverse_motorFrontLeft  = false;
bool is_reverse_motorBackLeft   = true;
bool is_reverse_motorFrontRight = false;
bool is_reverse_motorBackRight  = true;

void setup() {
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  move(200, 200);   // Move forward
  /* delay(2000);

  move(-200, 200);  // Turn left
  delay(2000);

  move(200, -200);  // Turn right
  delay(2000);

  move(-200, -200); // Move backward
  delay(2000);

  move(0, 0);       // Stop
  delay(2000); */
}

// Function to control motor speeds and directions
void move(int leftSpeed, int rightSpeed) {
  // Set absolute speed (0-255)
  int leftAbsSpeed = abs(leftSpeed);
  int rightAbsSpeed = abs(rightSpeed);

  // Apply speed to motors
  motorFrontLeft.setSpeed(leftAbsSpeed);
  motorBackLeft.setSpeed(leftAbsSpeed);
  motorFrontRight.setSpeed(rightAbsSpeed);
  motorBackRight.setSpeed(rightAbsSpeed);

  // Adjust direction based on is_reverse flag
  if (leftSpeed >= 0) {
    motorFrontLeft.run(is_reverse_motorFrontLeft ? BACKWARD : FORWARD);
    motorBackLeft.run(is_reverse_motorBackLeft ? BACKWARD : FORWARD);
  } else {
    motorFrontLeft.run(is_reverse_motorFrontLeft ? FORWARD : BACKWARD);
    motorBackLeft.run(is_reverse_motorBackLeft ? FORWARD : BACKWARD);
  }

  if (rightSpeed >= 0) {
    motorFrontRight.run(is_reverse_motorFrontRight ? BACKWARD : FORWARD);
    motorBackRight.run(is_reverse_motorBackRight ? BACKWARD : FORWARD);
  } else {
    motorFrontRight.run(is_reverse_motorFrontRight ? FORWARD : BACKWARD);
    motorBackRight.run(is_reverse_motorBackRight ? FORWARD : BACKWARD);
  }

  // Debug output
  /* Serial.print("Moving - Left Speed: ");
  Serial.print(leftSpeed);
  Serial.print(" | Right Speed: ");
  Serial.println(rightSpeed); */
}
