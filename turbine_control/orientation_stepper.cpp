#include "orientation_stepper.h"
#include <Arduino.h>

OrientationStepper::OrientationStepper(Stepper* stepper, OrientationPID* pid, int rpm) {
  m_stepper = stepper;
  m_pid = pid;
  m_state = OFF;
  m_steps = 0;
  m_rpm = rpm;
  m_stepper->setSpeed(100);
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

  switch (m_state) {
    case OFF:
      break;
    case CLOCKWISE_AUTO:
      m_stepper->step(1);
      break;
    case ANTI_CLOCKWISE_AUTO:
      m_stepper->step(-1);
      break;
    case PID:
      break;
    case CLOCKWISE_STEPS:
      if (m_steps > 0) {
        m_stepper->step(m_steps);
        m_steps = 0;
      }
      break;
    case ANTI_CLOCKWISE_STEPS:
      if (m_steps > 0) {
        m_stepper->step(-m_steps);
        m_steps = 0;
      }
      break;
  }
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
  m_rpm = rpm;
  if (rpm != m_rpm) {
    m_stepper->setSpeed(m_rpm);
  }
}
void OrientationStepper::addSteps(int steps) {
  m_steps += steps;
}