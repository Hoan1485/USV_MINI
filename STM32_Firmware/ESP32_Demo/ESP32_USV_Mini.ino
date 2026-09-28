#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_THINGSBOARD_IP_OR_URL";
const char* token = "YOUR_DEVICE_TOKEN";

WiFiClient espClient;
PubSubClient client(espClient);

// UART to STM32 (Serial2 on ESP32, e.g. RX=16, TX=17)
#define RXD2 16
#define TXD2 17

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  if (String(topic).startsWith("v1/devices/me/rpc/request/")) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return;

    String method = doc["method"];
    if (method == "setMode") {
      int mode = doc["params"];
      Serial2.print("CMD,MODE,");
      Serial2.println(mode);
    }
    else if (method == "feed") {
      Serial2.print("CMD,FEED\n");
    }
    else if (method == "setWaypoint") {
      float lat = doc["params"]["lat"];
      float lon = doc["params"]["lon"];
      Serial2.print("CMD,WP,");
      Serial2.print(lat, 6);
      Serial2.print(",");
      Serial2.println(lon, 6);
    }
    else if (method == "setSpeed") {
      int left = doc["params"]["left"];
      int right = doc["params"]["right"];
      Serial2.print("CMD,SPEED,");
      Serial2.print(left);
      Serial2.print(",");
      Serial2.println(right);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("USV_ESP32", token, NULL)) {
      Serial.println("connected");
      client.subscribe("v1/devices/me/rpc/request/+");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // Connect to STM32 UART3 (PB10, PB11)
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read Telemetry from STM32
  // Format: TEL,lat,lon,heading,temp,speed\n
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    if (data.startsWith("TEL,")) {
      int comma1 = data.indexOf(',');
      int comma2 = data.indexOf(',', comma1 + 1);
      int comma3 = data.indexOf(',', comma2 + 1);
      int comma4 = data.indexOf(',', comma3 + 1);
      int comma5 = data.indexOf(',', comma4 + 1);
      
      if (comma5 > 0) {
        String lat = data.substring(comma1 + 1, comma2);
        String lon = data.substring(comma2 + 1, comma3);
        String heading = data.substring(comma3 + 1, comma4);
        String temp = data.substring(comma4 + 1, comma5);
        String speed = data.substring(comma5 + 1);
        
        StaticJsonDocument<200> doc;
        doc["latitude"] = lat.toFloat();
        doc["longitude"] = lon.toFloat();
        doc["heading"] = heading.toFloat();
        doc["temperature"] = temp.toFloat();
        doc["speed"] = speed.toFloat();
        
        char jsonBuffer[512];
        serializeJson(doc, jsonBuffer);
        
        client.publish("v1/devices/me/telemetry", jsonBuffer);
        Serial.println("Published: " + String(jsonBuffer));
      }
    }
  }
}
