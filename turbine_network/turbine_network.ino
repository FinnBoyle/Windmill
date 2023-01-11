#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <FS.h>

// Update these with values suitable for your network.

const char* ssid = "SCMS_RESEARCH";
const char* password = "S0nny@AUT";
const char* mqtt_server = "172.16.24.115";

// const char* ssid = "2.4ghz";
// const char* password = "computer";
// const char* mqtt_server = "192.168.1.168";

//temporary settings before file storage implemented
//Default values
double dVoltage = 0;
int dStepperState = 0, dRPM= 100, dSteps = 0, dkp = 0, dki = 0, dkd = 0;
//settings pointers
const char* ID = "T1";
double* voltage = &dVoltage;
int* stepperState = &dStepperState;
int* rpm = &dRPM;
int* steps = &dSteps;
int* kp = &dkp;
int* ki = &dki;
int* kd = &dkd;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  char settingsTopic[strlen(ID) + strlen("SETTINGS_")];
  strcpy(settingsTopic, "SETTINGS_");
  strcat(settingsTopic, ID);
  if (String(topic) == String(settingsTopic)) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';  //null terminate string
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, message);
    if (!error) {
      *stepperState = doc["stepperState"];
      *rpm = doc["rpm"];
      *steps = doc["steps"];
      *kp = doc["kp"];
      *ki = doc["ki"];
      *kd = doc["kd"];
      updateControllerSettings();
      saveSettings();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      char topicName[strlen(ID) + strlen("SETTINGS_")];
      // topicName = malloc(strlen(ID) + strlen("SETTINGS_"));
      strcpy(topicName, "SETTINGS_");
      strcat(topicName, ID);
      client.subscribe(topicName);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  pinMode(BUILTIN_LED, OUTPUT);  // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.begin(9600);
  setup_wifi();
  digitalWrite(BUILTIN_LED, LOW);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  SPIFFS.begin();
  readSavedSettings();
}

void updateControllerSettings() {
  DynamicJsonDocument doc(1024);
  doc["type"] = "SETTINGS";
  doc["stepperState"] = *stepperState;
  doc["rpm"] = *rpm;
  doc["steps"] = *steps;
  doc["kp"] = *kp;
  doc["ki"] = *ki;
  doc["kd"] = *kd;
  serializeJson(doc, Serial);
  Serial.write('\n');
  *steps = 0;
}

bool saveSettings() {
  SPIFFS.remove("settings.json");
  File file = SPIFFS.open("settings.json", "w");
  if (!file) {
    return false;
  }

  DynamicJsonDocument doc(1024);
  doc["stepperState"] = *stepperState;
  doc["rpm"] = *rpm;
  doc["steps"] = *steps;
  doc["kp"] = *kp;
  doc["ki"] = *ki;
  doc["kd"] = *kd;

  if (serializeJson(doc, file) == 0) {
    return false;
  }

  file.close();

  return true;
}

bool readSavedSettings() {
  Serial.println("Retrieving settings from settings.json");
  File file = SPIFFS.open("settings.json", "r");
  if (!file) {
    Serial.println("settings.json not found!");
    return false;
  }

  // Extract each characters by one by one
  // while (file.available()) {
  //   Serial.print((char)file.read());
  // }
  // Serial.println();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, file);
  if (!error) {
    *stepperState = doc["stepperState"];
    *rpm = doc['rpm'];
    *steps = doc['steps'];
    *kp = doc["kp"];
    *ki = doc["ki"];
    *kd = doc["kd"];
  }

  // Close the file
  file.close();
  return true;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (Serial.available()) {
    char buffer[100] = "";
    Serial.readBytesUntil('\n', buffer, 100);
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, buffer);
    if (!error) {
      if (doc["type"] == "METRICS") {
        //Serial.println(": " + String(bufferCopy));
        DynamicJsonDocument turbineData(1024);
        turbineData["id"] = ID;
        turbineData["voltage"] = doc["voltage"];
        turbineData["stepperState"] = *stepperState;
        turbineData["rpm"] = *rpm;
        turbineData["kp"] = *kp;
        turbineData["ki"] = *ki;
        turbineData["kd"] = *kd;
        char dataBuffer[100] = "";

        serializeJson(turbineData, dataBuffer);
        client.publish("TURBINE_FEED", dataBuffer);
      } else if (doc["type"] == "INIT_REQ") {
        updateControllerSettings();
      }
    }
  }
}