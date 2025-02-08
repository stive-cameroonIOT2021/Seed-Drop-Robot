/* ESP32 AP Mode with Web D-Pad Controller and few buttons*/
#include <WiFi.h>
#include <WebServer.h>

#define RXD1 16
#define TXD1 17
HardwareSerial mySerial(1);

const char *ssid = "ESP32_Controller";
const char *password = "12345678";
WebServer server(80);

String direction = "s";
int speedVal = 0;
String state = "c";//Seed drop command
String mode = "m";  // Default mode: Manual ("m"), Auto ("a")
String pullState = "p";  // Default: Pull-Up ("p"), Pull-Down ("w")

void sendData() {
  String message = direction + "," + String(speedVal) + "," + state + "," + mode + "," + pullState;
  mySerial.println(message);
}

void handleControl() {
  if (server.hasArg("dir")) {
    direction = server.arg("dir");
  }
  if (server.hasArg("speed")) {
    speedVal = server.arg("speed").toInt();
  }
  if (server.hasArg("state")) {
    state = server.arg("state");
  }
  if (server.hasArg("mode")) {
    mode = server.arg("mode");
  }
  if (server.hasArg("pull")) {
    pullState = server.arg("pull");
  }
  sendData();
  server.send(200, "text/plain", "OK");
}

void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>ESP32 D-Pad</title>
    <style>
      .container { text-align: center; font-family: Arial; display: flex; flex-direction: column; align-items: center; justify-content: flex-start; height: 100vh; padding-top: 20px; }
      .grid { display: flex; flex-direction: column; align-items: center; gap: 20px; }
      .row { display: flex; justify-content: center; gap: 20px; }
      button { width: 100px; height: 100px; font-size: 24px; }
      #speedSlider { width: 300px; height: 30px; }
      h3 { font-size: 28px; }
      .toggleContainer { display: flex; justify-content: center; gap: 20px; margin-top: 20px; }
      .toggleButton { font-size: 24px; width: 140px; height: 60px; border: none; cursor: pointer; }
      #toggleButton { background-color: red; color: white; }
      #modeButton { background-color: blue; color: white; }
      #pullButton { background-color: orange; color: white; }
    </style>
  </head>
  <body>
    <div class="container">
      <h3>Speed: <span id="speedVal">0</span></h3>
      <input type="range" min="0" max="255" value="0" id="speedSlider" oninput="updateSpeed()">
      
      <div id='dpad'>
        <div class='row'><button onclick="sendDirection('f')">UP</button></div>
        <div class='row'>
          <button onclick="sendDirection('l')">LEFT</button>
          <button onclick="stopMotion()">STOP</button>
          <button onclick="sendDirection('r')">RIGHT</button>
        </div>
        <div class='row'><button onclick="sendDirection('b')">DOWN</button></div>
      </div>

      <div class="toggleContainer">
        <button id="toggleButton" class="toggleButton" onclick="toggleState()">CLOSE</button>
        <button id="modeButton" class="toggleButton" onclick="toggleMode()">MANUAL</button>
        <button id="pullButton" class="toggleButton" onclick="togglePull()">PULL-UP</button>
      </div>
    </div>

    <script>
      let direction = "s";
      let speedVal = 0;
      let state = "c";
      let mode = "m";
      let pullState = "p";

      function sendDirection(dir) {
        direction = dir;
        let speed = document.getElementById("speedSlider").value;
        fetch(`/control?dir=${direction}&speed=${speed}&state=${state}&mode=${mode}&pull=${pullState}`);
      }

      function stopMotion() {
        direction = "s";
        speedVal = 0;
        document.getElementById("speedSlider").value = 0;
        document.getElementById("speedVal").innerText = 0;
        fetch(`/control?dir=s&speed=0&state=${state}&mode=${mode}&pull=${pullState}`);
      }

      function updateSpeed() {
        speedVal = document.getElementById("speedSlider").value;
        document.getElementById("speedVal").innerText = speedVal;
        fetch(`/control?dir=${direction}&speed=${speedVal}&state=${state}&mode=${mode}&pull=${pullState}`);
      }

      function toggleState() {
        let button = document.getElementById("toggleButton");
        state = (state === "c") ? "o" : "c";
        button.style.backgroundColor = (state === "o") ? "green" : "red";
        button.innerText = (state === "o") ? "OPEN" : "CLOSE";
        fetch(`/control?dir=${direction}&speed=${speedVal}&state=${state}&mode=${mode}&pull=${pullState}`);
      }

      function toggleMode() {
        let button = document.getElementById("modeButton");
        mode = (mode === "m") ? "a" : "m";
        button.style.backgroundColor = (mode === "a") ? "green" : "blue";
        button.innerText = (mode === "a") ? "AUTO" : "MANUAL";
        fetch(`/control?dir=${direction}&speed=${speedVal}&state=${state}&mode=${mode}&pull=${pullState}`);
      }

      function togglePull() {
        let button = document.getElementById("pullButton");
        pullState = (pullState === "p") ? "w" : "p";
        button.style.backgroundColor = (pullState === "p") ? "orange" : "purple";
        button.innerText = (pullState === "p") ? "PULL-UP" : "PULL-DOWN";
        fetch(`/control?dir=${direction}&speed=${speedVal}&state=${state}&mode=${mode}&pull=${pullState}`);
      }
    </script>
  </body>
  </html>)rawliteral");
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, RXD1, TXD1);

  WiFi.softAP(ssid, password);
  server.on("/control", handleControl);
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
