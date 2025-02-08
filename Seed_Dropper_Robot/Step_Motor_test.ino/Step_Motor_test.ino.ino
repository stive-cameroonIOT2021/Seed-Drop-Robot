#include <AccelStepper.h>

// Define the stepper motors and the pins they will use
AccelStepper Step_seed(1, A1, A0);   // (Type:driver, STEP, DIR) - Stepper1
AccelStepper Step_dig(1, A3, A2);  // Stepper2

const int Step_Speed = 3000;

// Reverse state for each step motor
bool isReverseStep_seed = false;
bool isReverseStep_dig = false;

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  // Set initial speed values for the steppers
  Step_seed.setMaxSpeed(3000);
  Step_dig.setMaxSpeed(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Continuously run the motors
  Step_seed.runSpeed();
  Step_dig.runSpeed();

  if (Serial.available()) {
    char command = Serial.read();  // Read the input command
    
    // Handle only valid commands, ignore newline characters
    if (command != '\n' && command != '\r') {
      switch (command) {
        case 'f':  // Move forward
          moveForward();
          Serial.println("Moving forward");
          break;
        case 'b':  // Move backward
          moveBackward();
          Serial.println("Moving backward");
          break;
        case 's':  // Stop movement
          stopMoving();
          Serial.println("Stopped");
          break;
        default:
          Serial.println("Invalid command");
          break;
      }
    }
  }

}


// Function to set the speed considering the reverse state
void setWheelSpeed(AccelStepper &wheel, int speed, bool isReverse) {
  if (isReverse) {
    wheel.setSpeed(-speed);  // Reverse speed if reverse is true
  } else {
    wheel.setSpeed(speed);   // Normal speed otherwise
  }
}

// Movement functions
void moveForward() {
  setWheelSpeed(Step_seed, Step_Speed, isReverseStep_seed);
  setWheelSpeed(Step_dig, Step_Speed, isReverseStep_dig);
}

void moveBackward() {
  setWheelSpeed(Step_seed, -Step_Speed, isReverseStep_seed);
  setWheelSpeed(Step_dig, -Step_Speed, isReverseStep_dig);
}

void stopMoving() {
  setWheelSpeed(Step_seed, 0, isReverseStep_seed);
  setWheelSpeed(Step_dig, 0, isReverseStep_dig);
}