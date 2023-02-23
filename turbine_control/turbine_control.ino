#include <Stepper.h>
#include "orientation_stepper.h"
#include <ArduinoJson.h>
#include <SoftwareSerial.h>


const int stepsPerRevolution = 200;
String message = "";
bool msgReady = false;
unsigned long lastMetricsSend = 0;
unsigned long lastPIDSend = 0;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 9, 10, 11, 12);
OrientationPID* orientationPID = NULL;
OrientationStepper* orientationStepper = NULL;


const byte rxPin = 2;
const byte txPin = 3;
SoftwareSerial espSerial(rxPin, txPin);

//Default values
double dVoltage = 0;
int dState = 0, dRPM = 100, dSteps = 0, dkp = 0, dki = 0, dkd = 0;
//settings pointers
double* voltage = &dVoltage;
int* state = &dState;
int* rpm = &dRPM;
int* steps = &dSteps;
int* kp = &dkp;
int* ki = &dki;
int* kd = &dkd;


extern "C" char* sbrk(int incr);
void display_freeram() {
  Serial.print(F("- SRAM left: "));
  Serial.println(freeRam());
}

int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

double getVoltage() {
  double voltage = (analogRead(A0) * 5/1023.0);
  return voltage;
}
void setup() {
  // analogReference(EXTERNAL);
  analogReference(DEFAULT);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(9600);
  espSerial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  double volts = getVoltage();
  //every 2s send an initialization request or turbine metrics to ESP8266
  if (now - lastMetricsSend > 2000) {
    lastMetricsSend = now;
    if (orientationStepper == NULL) {
      DynamicJsonDocument doc(100);
      doc["type"] = "INIT_REQ";
      serializeJson(doc, espSerial);
      espSerial.write('\n');

    } else {
      DynamicJsonDocument doc(150);
      doc["type"] = "METRICS";
      doc["voltage"] = volts;
      doc["memory"] = freeRam();
      doc["rotation"] = orientationStepper->getRotation();
      serializeJson(doc, espSerial);
      espSerial.write('\n');
    }
  }

  if (espSerial.available()) {
    char buffer[150] = "";
    espSerial.readBytesUntil('\n', buffer, 150);
    char bufferCopy[150] = "";


    strcpy(bufferCopy, buffer);

    DynamicJsonDocument doc(150);
    DeserializationError error = deserializeJson(doc, buffer);
    if (error) {
      Serial.println("Deserialization Failed!");
      Serial.println(bufferCopy);
      //When error occurs clear buffer to ensure future messages are not impacted
      espSerial.end();
      espSerial.begin(9600);

    } else {
      if (doc["type"] == "SETTINGS") {
        Serial.println("Settings updated!");
        Serial.println(bufferCopy);


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
          *state = doc["stepperState"];
        }
        if (doc.containsKey("rpm")) {
          *rpm = doc["rpm"];
        }
        if (doc.containsKey("steps")) {
          *steps = doc["steps"];
        }
        if (orientationStepper == NULL) {
          orientationPID = new OrientationPID(1.5, *kp, *ki, *kd, 5, 5);
          orientationStepper = new OrientationStepper(&myStepper, orientationPID, 300, 5, 90);
          orientationStepper->setState(*state);
          Serial.println("INITIALIZED");
        } else {
          orientationPID->setConstants(*kp, *ki, *kd);
          orientationStepper->setState(*state);
          orientationStepper->setRPM(*rpm);
          orientationStepper->addSteps(*steps);
          //   Serial.print("Set RPM to: ");
          //   Serial.println(rpm);
        }
      }
    }
  }

  //Serial.println();
  if (orientationStepper != NULL) {
    // display_freeram();
    orientationStepper->update(volts);
    bool pidIntervalDone = (now - lastPIDSend) > orientationStepper->getInterval();
    if (orientationStepper->bufferFull() && pidIntervalDone) {
      lastPIDSend = now;
      DynamicJsonDocument doc(150);
      doc["type"] = "PID";
      doc["interval"] = orientationStepper->getInterval();

      float* rotationsArray = orientationStepper->getRotationHistory();
      JsonArray rotations = doc.createNestedArray("rotations");
      for (int i = 0; i < 5; i++) {
        rotations.add(rotationsArray[i]);
      }

      JsonArray errors = doc.createNestedArray("errors");
      float* errorsArray = orientationStepper->getErrorHistory();
      for (int i = 0; i < 5; i++) {
        errors.add(errorsArray[i]);
      }
      serializeJson(doc, espSerial);
      espSerial.write('\n');
      orientationStepper->resetBuffers();
    }
  }
}
