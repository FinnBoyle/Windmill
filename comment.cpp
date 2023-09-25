#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SoftwareSerial.h>

#define MYPORT_TX 13  // Define the TX pin for SoftwareSerial
#define MYPORT_RX 12  // Define the RX pin for SoftwareSerial

SoftwareSerial arduinoSerial;

// Wi-Fi credentials
const char* ssid = "SCMS_RESEARCH";
const char* password = "S0nny@AUT";
const char* mqtt_server = "172.16.24.139";

// Initialize variables
double dVoltage = 0;
int dStepperState = 0, dRPM = 20, dSteps = 0, dkp = 0, dki = 0, dkd = 0;

const char* ID = "T1";
double* voltage = &dVoltage;
int* stepperState = &dStepperState;
int* rpm = &dRPM;
int* steps = &dSteps;
int* kp = &dkp;
int* ki = &dki;
int* kd = &dkd;

iFiClient espClient;   // WiFi client instance for communication
PubSubClient client(espClient);  // MQTT client instance using the WiFi client
unsigned long lastMsg = 0;  // Timestamp for tracking the last message
char msg[MSG_BUFFER_SIZE];  // Buffer for storing MQTT messages
int value = 0;

// Function to set up Wi-Fi connection
void setup_wifi() {
  delay(10); // Small delay
  Serial.println(); // Print a newline to the serial monitor
  Serial.print("Connecting to "); // Print "Connecting to " to the serial monitor
  Serial.println(ssid); // Print the Wi-Fi SSID to the serial monitor

  WiFi.mode(WIFI_STA); // Set the ESP8266 in station mode for connecting to the Wi-Fi network
  WiFi.begin(ssid, password); // Attempt to connect to the specified Wi-Fi network with credentials

  while (WiFi.status() != WL_CONNECTED) { // Wait until the ESP8266 is connected to Wi-Fi
    delay(500); // Wait for 500 milliseconds
    Serial.print("."); // Print a dot (.) to the serial monitor to indicate the connection progress
  }

  randomSeed(micros()); // Seed the random number generator with the current microsecond value

  Serial.println(""); // Print a newline to the serial monitor
  Serial.println("WiFi connected"); // Indicate that Wi-Fi connection is successful
  Serial.println("IP address: "); // Print "IP address: " to the serial monitor
  Serial.println(WiFi.localIP()); // Print the local IP address assigned to the ESP8266
}


// Callback function for MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  // Construct the expected topic for this device
  char settingsTopic[strlen(ID) + strlen("SETTINGS_")];
  strcpy(settingsTopic, "SETTINGS_");
  strcat(settingsTopic, ID);

  // Check if the received MQTT topic matches the expected topic
  if (String(topic) == String(settingsTopic)) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    DynamicJsonDocument doc(200);
    DeserializationError error = deserializeJson(doc, message);

    // Check if JSON deserialization was successful
    if (!error) {
      // Extract and update controller settings from the received JSON
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
      // Update controller settings and save them
      updateControllerSettings(); // Update controller settings and send them over Serial
      saveSettings(); // Save the updated settings to a file
    }
  }
}

// Function to establish MQTT connection
void reconnect() {
  // Loop until the MQTT client is connected
  while (!client.connected()) {
    // Generate a unique client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect to the MQTT broker with the generated client ID
    if (client.connect(clientId.c_str())) {
      Serial.println("connected"); // Indicate successful connection to the serial monitor
      char topicName[strlen(ID) + strlen("SETTINGS_")];
      strcpy(topicName, "SETTINGS_");
      strcat(topicName, ID);
      
      // Subscribe to the MQTT topic for receiving settings updates
      client.subscribe(topicName);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state()); // Print the MQTT client's state to the serial monitor
      Serial.println(" try again in 5 seconds"); // Indicate a retry delay in the serial monitor
      delay(5000); // Wait for 5 seconds before attempting to reconnect
    }
  }
}


// Setup function
void setup() {
  pinMode(BUILTIN_LED, OUTPUT); // Set the built-in LED pin as an output
  digitalWrite(BUILTIN_LED, HIGH); // Turn off the built-in LED initially
  Serial.begin(9600); // Initialize the serial communication at 9600 baud
  arduinoSerial.begin(9600, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false); // Initialize SoftwareSerial for communication on custom ports

  setup_wifi(); // Call the function to set up the Wi-Fi connection
  digitalWrite(BUILTIN_LED, LOW); // Turn on the built-in LED to indicate Wi-Fi connection
  client.setServer(mqtt_server, 1883); // Configure MQTT client to connect to the MQTT server at the specified address and port
  client.setCallback(callback); // Set the callback function for handling MQTT messages
  SPIFFS.begin(); // Initialize SPIFFS (SPI Flash File System) for file storage
  readSavedSettings(); // Read and load previously saved controller settings from a file
}

// Function to update controller settings and send them via Serial
void updateControllerSettings() {
  DynamicJsonDocument doc(200); // Create a JSON document to hold controller settings
  doc["type"] = "SETTINGS"; // Set the JSON object type as "SETTINGS"
  doc["stepperState"] = *stepperState; // Add stepper state to the JSON document
  doc["rpm"] = *rpm; // Add RPM value to the JSON document
  doc["steps"] = *steps; // Add step count to the JSON document
  doc["kp"] = *kp; // Add proportional gain (kp) to the JSON document
  doc["ki"] = *ki; // Add integral gain (ki) to the JSON document
  doc["kd"] = *kd; // Add derivative gain (kd) to the JSON document
  
  serializeJson(doc, arduinoSerial); // Serialize the JSON document and send it via the SoftwareSerial
  arduinoSerial.write('\n'); // Send a newline character to mark the end of the JSON message
  *steps = 0; // Reset the step count to zero after sending the settings
}
// Function to save controller settings to SPIFFS (SPI Flash File System)
bool saveSettings() {
  SPIFFS.remove("settings.json"); // Remove any existing settings file
  File file = SPIFFS.open("settings.json", "w"); // Open a new settings file for writing
  if (!file) {
    return false; // If unable to open the file, return false to indicate failure
  }

  DynamicJsonDocument doc(200); // Create a JSON document to hold controller settings
  doc["stepperState"] = *stepperState; // Add stepper state to the JSON document
  doc["rpm"] = *rpm; // Add RPM value to the JSON document
  doc["steps"] = *steps; // Add step count to the JSON document
  doc["kp"] = *kp; // Add proportional gain (kp) to the JSON document
  doc["ki"] = *ki; // Add integral gain (ki) to the JSON document
  doc["kd"] = *kd; // Add derivative gain (kd) to the JSON document

  // Serialize the JSON document and write it to the settings file
  if (serializeJson(doc, file) == 0) {
    return false; // If serialization fails, return false to indicate failure
  }

  file.close(); // Close the settings file
  return true; // Return true to indicate successful saving of settings
}
// Function to read controller settings from SPIFFS (SPI Flash File System)
bool readSavedSettings() {
  File file = SPIFFS.open("settings.json", "r"); // Open the settings file for reading
  if (!file) {
    return false; // If unable to open the file, return false to indicate failure
  }

  DynamicJsonDocument doc(200); // Create a JSON document to hold the read settings
  DeserializationError error = deserializeJson(doc, file); // Deserialize the JSON data from the file
  if (!error) {
    // Extract and update controller settings from the JSON document
    *stepperState = doc["stepperState"];
    *rpm = doc["rpm"];
    *steps = doc["steps"];
    *kp = doc["kp"];
    *ki = doc["ki"];
    *kd = doc["kd"];
  }

  file.close(); // Close the settings file
  return true; // Return true to indicate successful reading and updating of settings
}
// Main loop
void loop() {
  if (!client.connected()) {
    reconnect(); // Attempt to reconnect to the MQTT broker if not connected
  }
  client.loop(); // Allow the MQTT client to process incoming messages

  if (arduinoSerial.available()) { // Check if there is data available from Serial
    char buffer[450] = "";
    arduinoSerial.readBytesUntil('\n', buffer, 450); // Read a line of data from Serial
    DynamicJsonDocument doc(450); // Create a JSON document to parse the received data
    char bufferCopy[450] = "";

    strcpy(bufferCopy, buffer); // Make a copy of the received data
    DeserializationError error = deserializeJson(doc, buffer); // Parse the received JSON data

    if (error) {
      Serial.println("Deserialization Failed!"); // Print a message indicating JSON parsing failure
      Serial.println(bufferCopy); // Print the original JSON data for debugging
      arduinoSerial.end(); // End Serial communication
      arduinoSerial.begin(9600, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false); // Reinitialize Serial communication
    } else {
      if (doc["type"] == "METRICS") {
        // Process and publish metric data
        DynamicJsonDocument turbineData(250); // Create a JSON document for turbine metric data
        turbineData["id"] = ID;
        turbineData["memory"] = doc["memory"];
        turbineData["voltage"] = doc["voltage"];
        turbineData["stepperState"] = *stepperState;
        turbineData["rpm"] = *rpm;
        turbineData["rotation"] = doc["rotation"];
        turbineData["kp"] = *kp;
        turbineData["ki"] = *ki;
        turbineData["kd"] = *kd;
        char dataBuffer[250] = "";
        serializeJson(turbineData, dataBuffer); // Serialize the metric data
        client.publish("TURBINE_FEED", dataBuffer); // Publish metric data to MQTT topic
      } else if (doc["type"] == "INIT_REQ") {
        // Request to update controller settings
        updateControllerSettings(); // Update and send controller settings
      } else if (doc["type"] == "PID") {
        // Process and publish PID data
        DynamicJsonDocument pidData(300); // Create a JSON document for PID data
        pidData["id"] = ID;
        pidData["interval"] = doc["interval"];
        pidData["rotations"] = doc["rotations"];
        pidData["errors"] = doc["errors"];
        char dataBuffer[300] = "";
        serializeJson(pidData, dataBuffer); // Serialize the PID data
        Serial.println(bufferCopy); // Print the received data for debugging
        Serial.println(dataBuffer); // Print the serialized PID data for debugging
        client.publish("PID_FEED", dataBuffer); // Publish PID data to MQTT topic
      }
    }
  }
}
