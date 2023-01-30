#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SoftwareSerial.h>

#define MYPORT_TX 13  //D7
#define MYPORT_RX 12  //D6

SoftwareSerial arduinoSerial;
// Update these with values suitable for your network.

// const char* ssid = "SCMS_RESEARCH";
// const char* password = "S0nny@AUT";
// const char* mqtt_server = "172.16.24.115";

const char* ssid = "2.4ghz";
const char* password = "computer";
const char* mqtt_server = "192.168.1.4";

//temporary settings before file storage implemented
//Default values
double dVoltage = 0;
int dStepperState = 0, dRPM = 100, dSteps = 0, dkp = 0, dki = 0, dkd = 0;
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
    DynamicJsonDocument doc(200);
    DeserializationError error = deserializeJson(doc, message);
    if (!error) {
      if (doc.containsKey("kp")) {
        *kp = doc["kp"];
      }
      if (doc.containsKey("ki")) {
        *ki = doc["ki"];
      }
      if (doc.containsKey("kd")) {
        *kd = doc["kd"];
      }
      if (doc.containsKey("stepperState")) {
        *stepperState = doc["stepperState"];
      }
      if (doc.containsKey("rpm")) {
        *rpm = doc["rpm"];
      }
      if (doc.containsKey("steps")) {
        *steps = doc["steps"];
      }
      updateControllerSettings();
      saveSettings();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Serial.print("Attempting MQTT connection...");
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
  arduinoSerial.begin(9600, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false);

  setup_wifi();
  digitalWrite(BUILTIN_LED, LOW);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  SPIFFS.begin();
  readSavedSettings();
}

void updateControllerSettings() {
  DynamicJsonDocument doc(200);
  doc["type"] = "SETTINGS";
  doc["stepperState"] = *stepperState;
  doc["rpm"] = *rpm;
  doc["steps"] = *steps;
  doc["kp"] = *kp;
  doc["ki"] = *ki;
  doc["kd"] = *kd;
  serializeJson(doc, arduinoSerial);
  arduinoSerial.write('\n');
  *steps = 0;
}

bool saveSettings() {
  SPIFFS.remove("settings.json");
  File file = SPIFFS.open("settings.json", "w");
  if (!file) {
    return false;
  }

  DynamicJsonDocument doc(200);
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
  // arduinoSerial.println("Retrieving settings from settings.json");
  File file = SPIFFS.open("settings.json", "r");
  if (!file) {
    // arduinoSerial.println("settings.json not found!");
    return false;
  }

  // Extract each characters by one by one
  // while (file.available()) {
  //   Serial.print((char)file.read());
  // }
  // Serial.println();

  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, file);
  if (!error) {
    *stepperState = doc["stepperState"];
    *rpm = doc["rpm"];
    *steps = doc["steps"];
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
  if (arduinoSerial.available()) {

    char buffer[450] = "";
    arduinoSerial.readBytesUntil('\n', buffer, 450);
    DynamicJsonDocument doc(450);
    char bufferCopy[450] = "";


    strcpy(bufferCopy, buffer);

    DeserializationError error = deserializeJson(doc, buffer);

    if (error) {

      Serial.println("Deserialization Failed!");
      Serial.println(bufferCopy);
      //When error occurs clear buffer to ensure future messages are not impacted
      arduinoSerial.end();
      arduinoSerial.begin(9600, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false);
    } else {
      if (doc["type"] == "METRICS") {

        //Serial.println(": " + String(bufferCopy));

        DynamicJsonDocument turbineData(200);
        turbineData["id"] = ID;
        turbineData["memory"] = doc["memory"];
        turbineData["voltage"] = doc["voltage"];
        turbineData["stepperState"] = *stepperState;
        turbineData["rpm"] = *rpm;
        turbineData["kp"] = *kp;
        turbineData["ki"] = *ki;
        turbineData["kd"] = *kd;
        char dataBuffer[200] = "";

        serializeJson(turbineData, dataBuffer);
        // Serial.println(dataBuffer);
        client.publish("TURBINE_FEED", dataBuffer);
      } else if (doc["type"] == "INIT_REQ") {
        updateControllerSettings();
      } else if (doc["type"] == "PID") {
        // Serial.println("GOT HERE!");
        DynamicJsonDocument pidData(500);
        pidData["id"] = ID;
        pidData["interval"] = doc["interval"];
        pidData["rotations"] = doc["rotations"];
        pidData["errors"] = doc["errors"];
        char dataBuffer[500] = "";

        serializeJson(pidData, dataBuffer);
        Serial.println(bufferCopy);
      
        Serial.println(dataBuffer);
        client.publish("PID_FEED", dataBuffer);
        

      }
    }
  }
}