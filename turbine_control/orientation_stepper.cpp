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
  m_lastPIDUpdate = 0;
}

double OrientationStepper::fakeVoltage() {
  return 1.5 * exp(-0.0001 * pow((m_rotation - m_fakeWindDir), 2));
}

void OrientationStepper::handleState() {

  // int val1 = digitalRead(m_pin1);
  // int val2 = digitalRead(m_pin2);

  // //if 11
  // if (val1 && val2) {
  //   m_state = PID;
  // //if 00
  // } else if (!val1 && !val2) {
  //   m_state = OFF;
  // //if 01
  // } else if (!val1 && val2) {
  //   m_state = CLOCKWISE;
  // //if 10
  // } else {
  //   m_state = ANTI_CLOCKWISE;

  // }
}

void OrientationStepper::handleSteps() {
  if (m_state == CLOCKWISE_AUTO) {
    m_stepper->step(2);
    rotate(2);
  } else if (m_state == ANTI_CLOCKWISE_AUTO) {
    m_stepper->step(-2);
    rotate(-2);
  } else if (m_state == PID_FAKE) {
    unsigned long now = millis();
    //every 2s send an initialization request or turbine metrics to ESP8266
    if (now - m_lastPIDUpdate > 333) {
      m_lastPIDUpdate = now;
      double voltage = fakeVoltage();
      double change = m_pid->rotationChange(voltage);
      // Serial.println(change);
      m_stepper->step(change);
      rotate(change);
      Serial.println(change);
    }
  } else if (m_state == PID) {
   unsigned long now = millis();
    //every 2s send an initialization request or turbine metrics to ESP8266
    if (now - m_lastPIDUpdate > 333) {
      m_lastPIDUpdate = now;
      double voltage = analogRead(A0) * 1.65 / 1023;
      // double change = m_pid->rotationChange(voltage);
      // // Serial.println(change);
      // m_stepper->step(change);
      // rotate(change);
      Serial.println(voltage);
    }
  } else if (m_state == CLOCKWISE_STEPS) {
    if (m_steps > 0) {
      m_stepper->step(m_steps);
      rotate(m_steps);
      m_steps = 0;
    }
  } else if (m_state == ANTI_CLOCKWISE_STEPS) {
    if (m_steps > 0) {
      m_stepper->step(-m_steps);
      rotate(-m_steps);
      m_steps = 0;
    }
  }
  // switch (m_state) {
  //   case OFF:
  //     break;
  //   case CLOCKWISE_AUTO:
  //     m_stepper->step(2);
  //     rotate(2);
  //     break;
  //   case ANTI_CLOCKWISE_AUTO:
  //     m_stepper->step(-2);
  //     rotate(-2);
  //     break;
  //   case PID:
  //     unsigned long now = millis();
  //     //every 2s send an initialization request or turbine metrics to ESP8266
  //     if (now - m_lastPIDUpdate > 333) {
  //       m_lastPIDUpdate = now;
  //       double voltage = fakeVoltage();
  //       double change = m_pid->rotationChange(voltage);
  //       // Serial.println(change);
  //       m_stepper->step(change);
  //       rotate(change);
  //     }
  //     break;
  //   case CLOCKWISE_STEPS:
  //     if (m_steps > 0) {
  //       m_stepper->step(m_steps);
  //       rotate(m_steps);
  //       m_steps = 0;
  //     } else {
  //       Serial.println("got here");
  //     }
  //     break;
  //   case ANTI_CLOCKWISE_STEPS:
  //     if (m_steps > 0) {
  //       m_stepper->step(-m_steps);
  //       rotate(-m_steps);
  //       m_steps = 0;
  //     }
  //     break;
  // }
  // Serial.println(m_state);
}

void OrientationStepper::update() {
  handleState();
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

void OrientationStepper::rotate(int steps) {
  m_rotation += 1.8 * steps;
  m_rotation = fmodf(m_rotation, 360);
}