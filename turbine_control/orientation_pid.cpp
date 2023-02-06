#include "orientation_pid.h"
OrientationPID::OrientationPID(double setpoint, double kp, double ki, double kd)
  : m_pid(kp, ki, kd, PID::Direct) {
  // OrientationPID::OrientationPID(double setpoint, double kp, double ki, double kd) : m_pid(kp, ki, kd, PID::Direct) {
  m_setpoint = setpoint;
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
  m_lastError = 10000;
  m_pid.Start(0, 0, setpoint);
}


void OrientationPID::setConstants(double kp, double ki, double kd) {
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
  m_pid.SetTunings(kp, ki, kd);
}


double OrientationPID::rotationChange(double input) {
  double error = m_pid.Run(input);
  if (error > m_lastError) {
    if (m_pid.GetDirection() == PID::Direct) {
      m_pid.SetControllerDirection(PID::Reverse);
    } else if (m_pid.GetDirection() == PID::Reverse) {
      m_pid.SetControllerDirection(PID::Direct);
    }
  }
  m_lastError = error;
  return error;
}
