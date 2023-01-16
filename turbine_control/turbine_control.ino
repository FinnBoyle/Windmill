#include <Stepper.h>
#include "orientation_stepper.h"
#include <ArduinoJson.h>
#include <SoftwareSerial.h>


const int stepsPerRevolution = 200;
String message = "";
bool msgReady = false;
unsigned long lastMsg = 0;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 9, 10, 11, 12);
OrientationPID* orientationPID = NULL;
OrientationStepper* orientationStepper = NULL;


const byte rxPin = 2;
const byte txPin = 3;
SoftwareSerial espSerial(rxPin, txPin);



extern "C" char* sbrk(int incr);
// void display_freeram() {
//   Serial.print(F("- SRAM left: "));
//   Serial.println(freeRam());
// }

// int freeRam() {
//   extern int __heap_start,*__brkval;
//   int v;
//   return (int)&v - (__brkval == 0  
//     ? (int)&__heap_start : (int) __brkval);  
// }


void setup() {
  analogReference(EXTERNAL);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(9600);
  espSerial.begin(9600);
}

void loop() {
  unsigned long now = millis();

  //every 2s send an initialization request or turbine metrics to ESP8266
  if (now - lastMsg > 2000) {
    lastMsg = now;

    if (orientationStepper == NULL) {
      DynamicJsonDocument doc(100);
      doc["type"] = "INIT_REQ";
      serializeJson(doc, espSerial);
      espSerial.write('\n');
    } else {
      DynamicJsonDocument doc(100);
      doc["type"] = "METRICS";
      doc["voltage"] = analogRead(A0) * 1.65 / 1023;
      serializeJson(doc, espSerial);
      espSerial.write('\n');
    }
  }

  if (espSerial.available()) {
    char buffer[100] = "";
    espSerial.readBytesUntil('\n', buffer, 100);
    char bufferCopy[100] = "";


    strcpy(bufferCopy, buffer);

    DynamicJsonDocument doc(100);
    DeserializationError error = deserializeJson(doc, buffer);
    if (error) {
      Serial.println("Deserialization Failed!");
      Serial.println(bufferCopy);

      //When error occurs clear buffer to ensure future messages are not impacted
      espSerial.end();
      espSerial.begin(9600);

    } else {
      if (doc["type"] == "SETTINGS") {
        double kp = doc["kp"];
        double ki = doc["ki"];
        double kd = doc["kd"];
        int state = doc["stepperState"];
        int rpm = doc["rpm"];
        int steps = doc["steps"];

        if (orientationStepper == NULL) {

          orientationPID = new OrientationPID(1.5, kp, ki, kd);
          orientationStepper = new OrientationStepper(&myStepper, orientationPID);
          orientationStepper->setState(state);
          Serial.println(" INITIALIZED");
        } else {
          Serial.println(bufferCopy);
          orientationPID->setConstants(kp, ki, kd);
          orientationStepper->setState(state);
          orientationStepper->setRPM(rpm);
          orientationStepper->addSteps(steps);
          //   Serial.print("Set RPM to: ");
          //   Serial.println(rpm);
        }
      }
    }

    espSerial.end();
    espSerial.begin(9600);
  }

  //Serial.println();
  if (orientationStepper != NULL) {
    // display_freeram();
    orientationStepper->update();
  }
}
