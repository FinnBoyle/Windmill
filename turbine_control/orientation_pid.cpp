#include "orientation_pid.h"

OrientationPID::OrientationPID(double setpoint, int kp, int ki, int kd) {
  m_setpoint = setpoint;
  m_input = 0;
  m_output = 0;
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
  m_pid = new PID(&m_input, &m_output, &m_setpoint, m_kp, m_ki, m_kd, DIRECT);
  m_pid.SetMode(AUTOMATIC);
}

void OrientationPID::setConstants(int kp, int ki, int kd) {
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
}


int OrientationPID::rotationChange(int input) {
  m_input = input;
  m_pid.Compute();
  return m_output;
}