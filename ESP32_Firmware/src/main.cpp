#include "config.h"
#include "web.h"
#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

WebServer server(80);
HardwareSerial STM32Serial(2);
String rxBuffer;

// Function prototypes
void sendCommand(const String &command);
void receiveSTM32();
void parseSTM32Frame(String frame);
void parseStatus(String frame);
void parseError(String frame);
int splitFrame(String frame, String fields[], int maxFields);
void handleRoot();
void handleCommand();
void handleStatus();

// Telemetry Data
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

// Gửi lệnh điều khiển xuống STM32 qua UART
void sendCommand(const String &command) {
  STM32Serial.print(command);
  STM32Serial.print('\n');
  if (!command.startsWith("MOTOR|")) {
    Serial.print("[TX] ");
    Serial.println(command);
  }
}

// Nhận dữ liệu UART từ STM32
void receiveSTM32() {
  while (STM32Serial.available()) {
    char c = STM32Serial.read();
    if (c == '\n') {
      rxBuffer.trim();
      if (rxBuffer.length() > 0) {
        parseSTM32Frame(rxBuffer);
      }
      rxBuffer = "";
    } else {
      if (rxBuffer.length() < 200) {
        rxBuffer += c;
      } else {
        rxBuffer = "";
        Serial.println("[ERROR] RX buffer overflow");
      }
    }
  }
}

// Phân tích khung truyền nhận từ STM32
void parseSTM32Frame(String frame) {
  Serial.print("[RX] ");
  Serial.println(frame);

  if (frame.startsWith("STATUS|")) {
    parseStatus(frame);
  } else if (frame.startsWith("ERROR|")) {
    parseError(frame);
  } else {
    Serial.println("[ERROR] Unknown frame type");
  }
}

// Phân tích bản tin STATUS định kỳ
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
  lastError = "";
}

// Phân tích bản tin ERROR
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

// Tách chuỗi theo ký tự '|'
int splitFrame(String frame, String fields[], int maxFields) {
  int count = 0;
  int start = 0;
  while (count < maxFields) {
    int separator = frame.indexOf('|', start);
    if (separator < 0) {
      fields[count++] = frame.substring(start);
      break;
    }
    fields[count++] = frame.substring(start, separator);
    start = separator + 1;
  }
  return count;
}

// HTTP handler phục vụ trang web
void handleRoot() {
  server.send_P(200, "text/html; charset=UTF-8", INDEX_HTML);
}

// HTTP handler nhận lệnh điều khiển
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
  if (command.length() > 100) {
    server.send(400, "text/plain", "Command too long");
    return;
  }
  sendCommand(command);
  server.send(200, "text/plain", "OK");
}

// HTTP handler trả về trạng thái JSON
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

void setup() {
  Serial.begin(115200);
  STM32Serial.begin(UART_BAUDRATE, SERIAL_8N1, STM32_RX_PIN, STM32_TX_PIN);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  Serial.println();
  Serial.print("WiFi SSID: ");
  Serial.println(AP_SSID);
  Serial.print("WiFi IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/command", HTTP_GET, handleCommand);
  server.on("/status", HTTP_GET, handleStatus);
  server.begin();
  Serial.println("HTTP server started");

  delay(500);
  sendCommand("MODE|MANUAL");
  sendCommand("STOP");
}

void loop() {
  receiveSTM32();
  server.handleClient();
}