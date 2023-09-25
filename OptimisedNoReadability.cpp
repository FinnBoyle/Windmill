#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SoftwareSerial.h>

#define MYPORT_TX 13  // TX Pin
#define MYPORT_RX 12  // RX Pin

SoftwareSerial arduinoSerial(MYPORT_RX, MYPORT_TX);
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "SCMS_RESEARCH";
const char* password = "S0nny@AUT";
const char* mqtt_server = "172.16.24.139";
const char* ID = "T1";

struct Settings {
  double voltage;
  int stepperState, rpm, steps, kp, ki, kd;
} settings;  // Consolidate settings into one struct for better manageability

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void updateControllerSettings();
bool saveSettings();
bool readSavedSettings();

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.begin(9600);
  arduinoSerial.begin(9600);
  setup_wifi();
  digitalWrite(BUILTIN_LED, LOW);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  SPIFFS.begin();
  readSavedSettings();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (arduinoSerial.available()) {
    char buffer[450] = "";
    arduinoSerial.readBytesUntil('\n', buffer, 450);
    DynamicJsonDocument doc(450);  // Reuse the same JsonDocument
    DeserializationError error = deserializeJson(doc, buffer);
    if (error) {
      arduinoSerial.end();
      arduinoSerial.begin(9600);
      continue;  // Skip the rest of the loop
    }
    const char* type = doc["type"];
    doc["id"] = ID;
    char dataBuffer[300];  // To hold serialized JSON
    
    if (strcmp(type, "METRICS") == 0) {
      updateJsonDocWithSettings(doc);
      serializeJson(doc, dataBuffer);
      client.publish("TURBINE_FEED", dataBuffer);
    } else if (strcmp(type, "INIT_REQ") == 0) {
      updateControllerSettings();
    } else if (strcmp(type, "PID") == 0) {
      serializeJson(doc, dataBuffer);
      client.publish("PID_FEED", dataBuffer);
    }
  }
}

// Implement other functions without change unless specified.
// Include setup_wifi(), callback(), reconnect(), updateControllerSettings(),
// saveSettings(), and readSavedSettings() as they are in the original code.
