#include "orientation_stepper.h"
#include <Arduino.h>
#include <math.h>

OrientationStepper::OrientationStepper(Stepper* stepper, OrientationPID* pid) {
  m_stepper = stepper;
  m_pid = pid;
  m_state = OFF;
  m_steps = 0;
  m_rpm = 100;
  m_fakeWindDir = 180;
  m_rotation = 0;
  m_stepper->setSpeed(m_rpm);
  m_lastMove = 0;
}

double OrientationStepper::fakeVoltage() {
  return 1.5 * exp(-0.0001 * pow((m_rotation - m_fakeWindDir), 2));
}


void OrientationStepper::handleSteps() {
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
    if (now - m_lastMove > 333) {
      m_lastMove = now;
      double voltage = fakeVoltage();
      double change = m_pid->rotationChange(voltage);
      // Serial.println(change);
      m_stepper->step(calculateSteps(change));
    }
  } else if (m_state == PID) {
    unsigned long now = millis();
    //every 2s send an initialization request or turbine metrics to ESP8266
    if (now - m_lastMove > 333) {
      m_lastMove = now;
      double voltage = analogRead(A0) * 1.65 / 1023;
       double change = m_pid->rotationChange(voltage);
      // Serial.println(change);
      m_stepper->step(calculateSteps(change));
      Serial.println(voltage);
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

void OrientationStepper::update() {
  handleSteps();
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

int OrientationStepper::calculateSteps(int degrees) {
  int steps = 0;
  int old_rotation = m_rotation;
  // degrees = 50
  //If new rotation exceeds 360 degrees go the same position within 0-360 range
  if ((m_rotation + degrees) > 360) {
    int overshoot = m_rotation + degrees - 360;  //e.g. degrees over 360
    m_rotation = overshoot;                      //rotation is the the same rotation as just the degrees over 360
    steps -= (old_rotation - overshoot) / 1.8;
    //If new rotation is less than 360 degrees go the  same position within 0-360 range
  } else if ((m_rotation + degrees) < 0) {
    int overshoot = m_rotation + degrees;
    m_rotation = 360 + overshoot;
    steps += (m_rotation - old_rotation) / 1.8;
    //Otherwise move normally
  } else {
    steps = degrees / 1.8;
    m_rotation = m_rotation + degrees;
  }
  return steps;
}