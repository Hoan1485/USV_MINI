#include "config.h"
#include "web.h"
#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

// ============================================================
// WIFI CONFIG
// ============================================================

WebServer server(80);

// ============================================================
// UART CONFIG
// ============================================================

HardwareSerial STM32Serial(2);

String rxBuffer;

// ============================================================
// FUNCTION PROTOTYPES
// ============================================================

void sendCommand(const String &command);
void receiveSTM32();
void parseSTM32Frame(String frame);
void parseStatus(String frame);
void parseError(String frame);
int splitFrame(String frame, String fields[], int maxFields);
void handleRoot();
void handleCommand();
void handleStatus();

// ============================================================
// TELEMETRY DATA
// ============================================================

String latitude = "0.000000";
String longitude = "0.000000";
String heading = "0.0";
String speed = "0.00";
String battery = "0.00";
String current = "0.00";
String temp = "0.0";
String feed = "0";
String mode = "MANUAL";
String gps = "0";

String lastError = "";

// ============================================================
// HTML DASHBOARD (Được nạp từ web.h)
// ============================================================

// ============================================================
// SEND COMMAND TO STM32
// ============================================================

void sendCommand(const String &command) {
  STM32Serial.print(command);
  STM32Serial.print('\n');

  // Không in log cổng Serial USB đối với lệnh MOTOR liên tục để tránh nghẽn
  // CPU/UART
  if (!command.startsWith("MOTOR|")) {
    Serial.print("[TX] ");
    Serial.println(command);
  }
}

// ============================================================
// RECEIVE UART
// ============================================================

void receiveSTM32() {
  while (STM32Serial.available()) {
    char c = STM32Serial.read();

    // ----------------------------------------------
    // End of frame
    // ----------------------------------------------

    if (c == '\n') {
      rxBuffer.trim();

      if (rxBuffer.length() > 0) {
        parseSTM32Frame(rxBuffer);
      }

      rxBuffer = "";
    }

    // ----------------------------------------------
    // Normal character
    // ----------------------------------------------

    else {
      if (rxBuffer.length() < 200) {
        rxBuffer += c;
      }

      else {
        rxBuffer = "";

        Serial.println("[ERROR] RX buffer overflow");
      }
    }
  }
}

// ============================================================
// PARSE STM32 FRAME
// ============================================================

void parseSTM32Frame(String frame) {
  Serial.print("[RX] ");
  Serial.println(frame);

  if (frame.startsWith("STATUS|")) {
    parseStatus(frame);
  }

  else if (frame.startsWith("ERROR|")) {
    parseError(frame);
  }

  else {
    Serial.println("[ERROR] Unknown frame type");
  }
}

// ============================================================
// PARSE STATUS
// ============================================================

void parseStatus(String frame) {
  String fields[11];

  int count = splitFrame(frame, fields, 11);

  if (count != 11) {
    Serial.print("[STATUS] Invalid field count: ");

    Serial.println(count);

    return;
  }

  latitude = fields[1];

  longitude = fields[2];

  heading = fields[3];

  speed = fields[4];

  battery = fields[5];

  current = fields[6];

  temp = fields[7];

  feed = fields[8];

  mode = fields[9];

  gps = fields[10];

  // New valid status clears old error
  lastError = "";
}

// ============================================================
// PARSE ERROR
// ============================================================

void parseError(String frame) {
  int separator = frame.indexOf('|');

  if (separator < 0) {
    Serial.println("[ERROR] Invalid ERROR frame");

    return;
  }

  lastError = frame.substring(separator + 1);

  Serial.print("[STM32 ERROR] ");

  Serial.println(lastError);
}

// ============================================================
// SPLIT FRAME
// ============================================================

int splitFrame(String frame, String fields[], int maxFields) {
  int count = 0;

  int start = 0;

  while (count < maxFields) {
    int separator = frame.indexOf('|', start);

    // ----------------------------------------------
    // Last field
    // ----------------------------------------------

    if (separator < 0) {
      fields[count++] = frame.substring(start);

      break;
    }

    // ----------------------------------------------
    // Normal field
    // ----------------------------------------------

    fields[count++] = frame.substring(start, separator);

    start = separator + 1;
  }

  return count;
}

// ============================================================
// HTTP ROOT
// ============================================================

void handleRoot() {
  server.send_P(200, "text/html; charset=UTF-8", INDEX_HTML);
}

// ============================================================
// HTTP COMMAND
// ============================================================

void handleCommand() {
  if (!server.hasArg("cmd")) {
    server.send(400, "text/plain", "Missing cmd");

    return;
  }

  String command = server.arg("cmd");

  command.trim();

  if (command.length() == 0) {
    server.send(400, "text/plain", "Empty command");

    return;
  }

  // ----------------------------------------------
  // Basic command length protection
  // ----------------------------------------------

  if (command.length() > 100) {
    server.send(400, "text/plain", "Command too long");

    return;
  }

  sendCommand(command);

  server.send(200, "text/plain", "OK");
}

// ============================================================
// HTTP STATUS
// ============================================================

void handleStatus() {
  String json;
  json.reserve(220);
  json = "{\"lat\":\"" + latitude + "\",";
  json += "\"lon\":\"" + longitude + "\",";
  json += "\"heading\":\"" + heading + "\",";
  json += "\"speed\":\"" + speed + "\",";
  json += "\"battery\":\"" + battery + "\",";
  json += "\"current\":\"" + current + "\",";
  json += "\"temp\":\"" + temp + "\",";
  json += "\"feed\":\"" + feed + "\",";
  json += "\"mode\":\"" + mode + "\",";
  json += "\"gps\":\"" + gps + "\",";
  json += "\"error\":\"" + lastError + "\"}";

  server.send(200, "application/json", json);
}

// ============================================================
// SETUP
// ============================================================

void setup() {
  // --------------------------------------------------------
  // Serial Monitor
  // --------------------------------------------------------

  Serial.begin(115200);

  // --------------------------------------------------------
  // UART to STM32 / Simulator
  // --------------------------------------------------------

  STM32Serial.begin(UART_BAUDRATE, SERIAL_8N1, STM32_RX_PIN, STM32_TX_PIN);

  // --------------------------------------------------------
  // Wi-Fi Access Point
  // --------------------------------------------------------

  WiFi.mode(WIFI_AP);

  WiFi.softAP(AP_SSID, AP_PASSWORD);

  Serial.println();

  Serial.println("================================");

  Serial.println("       USV MINI DASHBOARD");

  Serial.println("================================");

  Serial.print("WiFi SSID: ");

  Serial.println(AP_SSID);

  Serial.print("WiFi IP: ");

  Serial.println(WiFi.softAPIP());

  // --------------------------------------------------------
  // HTTP routes
  // --------------------------------------------------------

  server.on("/", HTTP_GET, handleRoot);

  server.on("/command", HTTP_GET, handleCommand);

  server.on("/status", HTTP_GET, handleStatus);

  // --------------------------------------------------------
  // Start web server
  // --------------------------------------------------------

  server.begin();

  Serial.println("HTTP server started");

  // --------------------------------------------------------
  // Safe startup
  // --------------------------------------------------------

  delay(500);

  sendCommand("MODE|MANUAL");

  sendCommand("STOP");
}

// ============================================================
// LOOP
// ============================================================

void loop() {
  // Receive STATUS / ERROR
  receiveSTM32();

  // Process HTTP requests
  server.handleClient();
}