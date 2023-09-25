#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SoftwareSerial.h>

#define MYPORT_TX 13  // Define the TX pin for SoftwareSerial
#define MYPORT_RX 12  // Define the RX pin for SoftwareSerial

SoftwareSerial arduinoSerial(MYPORT_RX, MYPORT_TX);  // Initialize SoftwareSerial

const char* ssid = "SCMS_RESEARCH";
const char* password = "S0nny@AUT";
const char* mqtt_server = "172.16.24.139";
const char* ID = "T1";

struct ControllerSettings {
  double voltage;
  int stepperState;
  int rpm;
  int steps;
  int kp;
  int ki;
  int kd;
} settings;

WiFiClient espClient;  // WiFi client instance for communication
PubSubClient client(espClient);  // MQTT client instance using the WiFi client

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void updateControllerSettings();
bool saveSettings();
bool readSavedSettings();
void processIncomingData();

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
    processIncomingData();
  }
}

void setup_wifi() {
  // Your original code for WiFi setup remains unchanged.
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Your original code for MQTT callback remains unchanged.
}

void reconnect() {
  // Your original code for MQTT reconnection remains unchanged.
}

void updateControllerSettings() {
  DynamicJsonDocument doc(200);
  doc["type"] = "SETTINGS";
  doc["stepperState"] = settings.stepperState;
  doc["rpm"] = settings.rpm;
  doc["steps"] = settings.steps;
  doc["kp"] = settings.kp;
  doc["ki"] = settings.ki;
  doc["kd"] = settings.kd;

  serializeJson(doc, arduinoSerial);
  arduinoSerial.write('\n');
  settings.steps = 0;
}

bool saveSettings() {
  // Your original code for saving settings remains unchanged.
}

bool readSavedSettings() {
  // Your original code for reading saved settings remains unchanged.
}

void processIncomingData() {
  // This is the refactored code for processing incoming data.
  // Add your logic here for handling JSON and MQTT.
  // Your original code for this part can be moved here.
}
