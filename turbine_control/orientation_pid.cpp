#include "orientation_pid.h"
OrientationPID::OrientationPID(double setpoint, double kp, double ki, double kd) : m_pid(kp, ki, kd, PID::Direct) {
// OrientationPID::OrientationPID(double setpoint, double kp, double ki, double kd) : m_pid(kp, ki, kd, PID::Direct) {
  m_setpoint = setpoint;
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
  m_pid.Start(0, 0, setpoint);
  
}


void OrientationPID::setConstants(double kp, double ki, double kd) {
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
}


double OrientationPID::rotationChange(double input) {
  return m_pid.Run(input);
}