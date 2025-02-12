/* ----------------------------------------------------------------------------------------------------- */
#include <SoftwareSerial.h>//UART lib
#include <AccelStepper.h>//Step motor lib
/* -----------------------------------------------UART Setup------------------------------------------------------ */
// Define SoftwareSerial for ESP32 communication
SoftwareSerial mySerial(11, 12);//11,12 // RX, TX
// Global variables to store parsed data
String direction, speed, state1, mode="m", state2;
/* -------------------------------------Step motors setup---------------------------------------------------------------- */
// Define the stepper motors and the pins they will use
AccelStepper Step_seed(1, A1, A0);   // (Type:driver, STEP, DIR) - Stepper1
AccelStepper Step_dig(1, A3, A2);  // Stepper2

const int Step_Speed = 900;

// Reverse state for each step motor
bool isReverseStep_seed = false;
bool isReverseStep_dig = false;
/* ------------------------------------DC motors setup----------------------------------------------------------------- */
// Define motor control pins for Port D
#define LEFT_MOTOR_IN1  3  // Arduino Pin 3
#define LEFT_MOTOR_IN2  4  // Arduino Pin 4
#define RIGHT_MOTOR_IN1 5  // Arduino Pin 5
#define RIGHT_MOTOR_IN2 6  // Arduino Pin 6

// PWM pins for speed control
#define LEFT_PWM_PIN  9
#define RIGHT_PWM_PIN 10

#define MAX_SPEED 200

// Reverse motor flags
bool is_reverse_left = false;
bool is_reverse_right = true;

// Speed settings for the motors
int baseSpeed = 100;
/* ----------------------------------------------------------------------------------------------------- */
const int T_drop_seed = 6000;  // Time for dropping seeds in milliseconds
const int T_move = 2000;      // Time for moving the car in milliseconds
unsigned long previousMillis = 0;  // Store the last time the action was executed
unsigned long currentMillis = 0;   // Current time
/* ----------------------------------------------------------------------------------------------------- */
enum State { MOVING, DROPPING_SEEDS };
State currentState = MOVING;
/* ----------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------- */

//-----------------------FUNCTIONS--------------------------------------
void parseCommand(String command);

void setWheelSpeed(AccelStepper &wheel, int speed, bool isReverse);

void handleCommand(String direction, String speed, String state1_, String state2_);

void setupPWM();

void move(int leftSpeed, int rightSpeed);

void setup() {
  //-------------------------------------------------------------
  Serial.begin(115200);
  //-------------------------------------------------------------
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);
  pinMode(LEFT_PWM_PIN, OUTPUT);
  pinMode(RIGHT_PWM_PIN, OUTPUT);
  //-------------------------------------------------------------
  //-------------------------------------------------------------
  mySerial.begin(9600); // Baud rate for ESP32 communication
  Serial.println("Arduino is ready to receive commands");
  //-------------------------------------------------------------
  // Set initial speed values for the steppers
  Step_seed.setMaxSpeed(3000);
  Step_dig.setMaxSpeed(3000);
  //-------------------------------------------------------------
  //-------------------------------------------------------------
  //-------------------------------------------------------------
  //-------------------------------------------------------------


delay(2000);
}

void loop() {
  // Read incoming data from esp32 via UART setup pins
  if (mySerial.available() > 0) {
    String command = mySerial.readStringUntil('\n');
    command.trim(); // Remove unwanted spaces or newline characters

    /* direction = "";
    speed = "";
    state1 = "";
    mode = "";
    state2 = ""; */

    // Process and clean data
    parseCommand(command);
  }

  // Continuously run the motors
  Step_seed.runSpeed();
  Step_dig.runSpeed();

  currentMillis = millis();

if(mode == "a"){
  setWheelSpeed(Step_dig, 0, isReverseStep_dig);
  switch (currentState) {
    case MOVING:
      if (currentMillis - previousMillis >= T_move) {
        // Time to stop moving and start dropping seeds
        previousMillis = currentMillis;  // Reset the timer
        currentState = DROPPING_SEEDS;
      } else {
        setWheelSpeed(Step_seed, 0, isReverseStep_seed);
        // Move the car
        move(baseSpeed,baseSpeed);
      }
      break;

    case DROPPING_SEEDS:
      if (currentMillis - previousMillis >= T_drop_seed) {
        // Time to stop dropping seeds and switch back to moving
        previousMillis = currentMillis;  // Reset the timer
        currentState = MOVING;
      } else {
        // Drop seeds
        move(0,0);
        setWheelSpeed(Step_seed, Step_Speed, isReverseStep_seed);
      }
      break;
  }
}

else if (mode == "m") {
  handleCommand(direction, speed, state1, state2);
}
  




}


//-----------------------FUNCTIONS--------------------------------------
// Function to control motor speeds and directions
void move(int leftSpeed, int rightSpeed) {
  if (is_reverse_left) leftSpeed = -leftSpeed;
  if (is_reverse_right) rightSpeed = -rightSpeed;

  // Left motor control
  if (leftSpeed == 0) {
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
  } else if (leftSpeed < 0) {
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, HIGH);
  } else {
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
  }

  leftSpeed = constrain(abs(leftSpeed), 0, 255);
  analogWrite(LEFT_PWM_PIN, leftSpeed);

  // Right motor control
  if (rightSpeed == 0) {
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
  } else if (rightSpeed < 0) {
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  } else {
    digitalWrite(RIGHT_MOTOR_IN1, HIGH);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
  }

  rightSpeed = constrain(abs(rightSpeed), 0, 255);
  analogWrite(RIGHT_PWM_PIN, rightSpeed);
}


void handleCommand(String direction, String speed, String state1_, String state2_) {
  int speedValue = speed.toInt();

    if (state1_ == "c") {
      setWheelSpeed(Step_seed, 0, isReverseStep_seed);
    } else if (state1_ == "o") {
      setWheelSpeed(Step_seed, Step_Speed, isReverseStep_seed);
    }

    if (state2_ == "p") {
      setWheelSpeed(Step_dig, -3000, isReverseStep_dig);
    } else if (state2_ == "w") {
      setWheelSpeed(Step_dig, 3000, isReverseStep_dig);
    }

  // Switch case based on the direction
  switch(direction.charAt(0)) {
    case 's': // stop command 
    move(0, 0);
      break;

    case 'b': // backward command 
    move(-speedValue, -speedValue);
      break;

    case 'f': // forward command
    move(speedValue, speedValue);
      break;

    case 'r': //Turn right command
    move(speedValue, -speedValue);
      break;

    case 'l': //Turn left command
    move(-speedValue, speedValue);
      break;

    default:
      Serial.println("Unknown direction received: " + direction);
      break;
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

void parseCommand(String command) {
  if (command.length() == 0) return; // Ignore empty commands

  int firstComma = command.indexOf(',');
  int secondComma = command.indexOf(',', firstComma + 1);
  int thirdComma = command.indexOf(',', secondComma + 1);
  int fourthComma = command.indexOf(',', thirdComma + 1);

  if (firstComma == -1 || secondComma == -1 || thirdComma == -1 || fourthComma == -1) {
    Serial.println("Invalid command format: " + command);
    return;
  }

  // Assign values to global variables
  direction = command.substring(0, firstComma);
  speed = command.substring(firstComma + 1, secondComma);
  state1 = command.substring(secondComma + 1, thirdComma);
  mode = command.substring(thirdComma + 1, fourthComma);
  state2 = command.substring(fourthComma + 1);

  // Print cleaned data
  /* Serial.print("Direction: ");
  Serial.print(direction);
  Serial.print(", Speed: ");
  Serial.print(speed);
  Serial.print(", State1: ");
  Serial.print(state1);
  Serial.print(", Mode: ");
  Serial.print(mode);
  Serial.print(", State2: ");
  Serial.println(state2); */
}
