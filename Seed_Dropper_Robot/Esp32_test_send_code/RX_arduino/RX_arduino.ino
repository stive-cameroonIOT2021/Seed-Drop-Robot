#include <SoftwareSerial.h>

// Define SoftwareSerial for ESP32 communication
SoftwareSerial mySerial(9, 10); // RX, TX

// Global variables to store parsed data
String direction, speed, state1, mode, state2;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600); // Baud rate for ESP32 communication
  Serial.println("Arduino is ready to receive commands");
}

void loop() {
  if (mySerial.available() > 0) {
    String command = mySerial.readStringUntil('\n');
    command.trim(); // Remove unwanted spaces or newline characters

    // Process and clean data
    parseCommand(command);

    // Print parsed data
    printParsedData();
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
}

void printParsedData() {
  Serial.print("Direction: ");
  Serial.print(direction);
  Serial.print(", Speed: ");
  Serial.print(speed);
  Serial.print(", State1: ");
  Serial.print(state1);
  Serial.print(", Mode: ");
  Serial.print(mode);
  Serial.print(", State2: ");
  Serial.println(state2);
}
