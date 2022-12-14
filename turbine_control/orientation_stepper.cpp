#include "orientation_stepper.h"
#include <Arduino.h>

OrientationStepper::OrientationStepper(Stepper* stepper, OrientationPID* pid) {
  m_stepper = stepper;
  m_pid = pid;
  m_state = OFF;
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
    case CLOCKWISE:
      m_stepper->step(1);
      break;
    case ANTI_CLOCKWISE:
      m_stepper->step(-1);
      break;
    case PID:
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