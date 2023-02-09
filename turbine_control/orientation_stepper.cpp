#include "orientation_stepper.h"
#include <Arduino.h>
#include <math.h>
#define PI 3.14159265
OrientationStepper::OrientationStepper(Stepper* stepper, OrientationPID* pid,  int pidInterval, int bufferSize, double rotationTolerance) {
  m_stepper = stepper;
  m_pid = pid;
  m_state = OFF;
  m_steps = 0;
  m_rpm = 20;
  m_fakeWindDir = 180;
  m_rotation = 230;
  m_stepper->setSpeed(m_rpm);
  m_lastMove = 0;
  m_rotationTolerance = rotationTolerance;
  m_pidInterval = pidInterval;
  m_bufferSize = bufferSize;
  m_pidErrorHistory = new float[m_bufferSize];
  m_rotationHistory = new int[m_bufferSize];
  m_bufferIndex = 0;
}

// double OrientationStepper::fakeVoltage() {
//   return 1.5 * exp(-0.0001 * pow((m_rotation - m_fakeWindDir), 2));
// }

double OrientationStepper::fakeVoltage() {
  return sin(m_rotation/57.3)/2.0;
}

void OrientationStepper::update(double volts) {
  if (m_state == CLOCKWISE_AUTO) {
    unsigned long now = millis();
    if (now - m_lastMove > 20) {
      m_lastMove = now;
      int steps = calculateSteps(20);
      m_stepper->step(steps);
    }
  } else if (m_state == ANTI_CLOCKWISE_AUTO) {
    unsigned long now = millis();
    if (now - m_lastMove > 50) {
      m_lastMove = now;
      int steps = calculateSteps(-20);
      m_stepper->step(steps);
    }
  }else if (m_state == PID_FAKE) {
    unsigned long now = millis();
    //every 2s send an initialization request or turbine metrics to ESP8266
    if (now - m_lastMove > m_pidInterval) {
      m_lastMove = now;
      double voltage = fakeVoltage();
      double change = m_pid->rotationChange(voltage);
      // Serial.println(change);
      m_pidErrorHistory[m_bufferIndex] = change;
      m_rotationHistory[m_bufferIndex] = m_rotation;
      m_bufferIndex++;
      m_stepper->step(calculateSteps(change));
      Serial.print("Voltage: ");
      Serial.print(voltage);
      Serial.print(" change: ");
      Serial.println(change);
    }
  } else if (m_state == PID) {
    unsigned long now = millis();
    //every 2s send an initialization request or turbine metrics to ESP8266
    if (now - m_lastMove > m_pidInterval) {
      m_lastMove = now;
      double voltage = volts;
      double change = m_pid->rotationChange(voltage);
      // Serial.println(change);
      m_pidErrorHistory[m_bufferIndex] = change;
      m_rotationHistory[m_bufferIndex] = m_rotation;
      m_bufferIndex++;
      m_stepper->step(calculateSteps(change));
      Serial.print("Voltage: ");
      Serial.print(voltage);
      Serial.print(" change: ");
      Serial.println(change);
    }
  } else if (m_state == CLOCKWISE_STEPS) {
    if (m_steps > 0) {
      m_stepper->step(calculateSteps(m_steps * 1.8));
      m_steps = 0;
    }
  } else if (m_state == ANTI_CLOCKWISE_STEPS) {
    if (m_steps > 0) {
      m_stepper->step(calculateSteps(-m_steps * 1.8));
      m_steps = 0;
    }
  }
}

StepperState OrientationStepper::getState() {
  return m_state;
}

void OrientationStepper::setState(int state) {
  m_state = state;
}

void OrientationStepper::setRPM(int rpm) {
  if (rpm != m_rpm) {
    m_rpm = rpm;

    m_stepper->setSpeed(m_rpm);
  }
}
void OrientationStepper::addSteps(int steps) {
  m_steps += steps;
}

int OrientationStepper::calculateSteps(double degrees) {
  int steps = 0;
  double old_rotation = m_rotation;
  // degrees = 50
  //If new rotation exceeds  (360 + m_rotationTolerance) degrees go the same position within 0-(360 + m_rotationTolerance) range
  if ((m_rotation + degrees) > (360 + m_rotationTolerance)) {
    double overshoot = m_rotation + degrees - 360;  //e.g. degrees over 360
    m_rotation = overshoot;                      //rotation is the the same rotation as just the degrees over 360
    steps -= (old_rotation - overshoot) / 1.8;
    //If new rotation is less than  (360 + m_rotationTolerance) degrees go the  same position within 0-(360 + m_rotationTolerance) range
  } else if ((m_rotation + degrees) < (0 - m_rotationTolerance)) {
    double overshoot = m_rotation + degrees;
    m_rotation =  360 + overshoot;
    steps += (m_rotation - old_rotation) / 1.8;
    //Otherwise move normally
  } else {
    steps = degrees / 1.8;
    m_rotation = m_rotation + degrees;
  }
  if (steps == 0 && degrees > 0) {
    m_rotation = m_rotation + 1.8;
    return 1;
  }
  return steps;
}

  double OrientationStepper::getRotation() {
    return m_rotation;
  }

 void OrientationStepper::resetBuffers() {
   m_bufferIndex = 0;
 }
  bool OrientationStepper::bufferFull() {
    return m_bufferIndex >= m_bufferSize;
  }
  float* OrientationStepper::getErrorHistory() {
    return m_pidErrorHistory;
  }
  int* OrientationStepper::getRotationHistory() {
    return m_rotationHistory;
  }
  int OrientationStepper::getInterval() {
    return m_pidInterval;
  }
