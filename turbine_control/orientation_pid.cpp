#include "orientation_pid.h"

OrientationPID::OrientationPID(double setpoint, double input, double output, int kp, int ki, int kd) {
  m_setpoint = setpoint;
  m_input = input;
  m_output = output;
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
}

void OrientationPID::setConstants(int kp, int ki, int kd) {
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
}


int OrientationPID::rotationChange() {
  
}