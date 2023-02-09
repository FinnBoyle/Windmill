#include "orientation_pid.h"
#include <math.h>
#include <Arduino.h>

OrientationPID::OrientationPID(double setpoint, double kp, double ki, double kd, double reversalTheshold, int bufferSize)
  : m_pid(kp, ki, kd, PID::Direct) {
  // OrientationPID::OrientationPID(double setpoint, double kp, double ki, double kd) : m_pid(kp, ki, kd, PID::Direct) {
  m_setpoint = setpoint;
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
  m_pid.Start(0, 0, setpoint);
  m_reversalTheshold = reversalTheshold;

  //buffer variables
  m_bufferIndex = 0;
  m_bufferLen = 0;
  m_bufferSize = bufferSize;
  m_errorBuffer = new double[m_bufferSize];

  //moving avg calculations
  m_errorTotal = 0;
  m_prevAvgError = 0; 
  m_AvgError = 0;
}


void OrientationPID::setConstants(double kp, double ki, double kd) {
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
  m_pid.SetTunings(kp, ki, kd);
}

void OrientationPID::calculateNextError(double error) {
  double oldValue = m_errorBuffer[m_bufferIndex];
  m_errorBuffer[m_bufferIndex] = error;
  m_bufferIndex = (m_bufferIndex + 1) % m_bufferSize;
  if (m_bufferLen < m_bufferSize - 1) {
    m_bufferLen++;
  }

  m_errorTotal -= oldValue;
  m_errorTotal += error;

  m_prevAvgError = m_AvgError;
  m_AvgError = m_errorTotal / m_bufferLen;
}

double OrientationPID::rotationChange(double input) {
  double error = m_pid.Run(input);
  this->calculateNextError(error);
  Serial.print("Moving Avg Error: ");
  Serial.println(m_AvgError);
  //if error is increasing by more than the reversal theshold then change direction
  if ((m_AvgError - m_prevAvgError) > m_reversalTheshold) {
    Serial.println("Direction Change!");

    m_reverse = !m_reverse;
  }
  //If in reverse return error as a negative
  if (m_reverse) {
    return -error;
  } else {
    return error;
  }
}


// double OrientationPID::rotationChange(double input) {
//   double error = m_pid.Run(input);

//   //If going in reverse and error is decreasing when last error is less than zero, change direction
//   //e.g. lastError = -5 then error = -6 but not 5 then -6
//   if (m_reverse && -error < m_lastError && m_lastError < 0) {
//     m_reverse = false;
//     //If going forwards and error is increasing when last error is greater than zero, change direction
//     //e.g. lastError = 5 then error = 6 but not -5 then 6
//   } else if (!m_reverse && error > m_lastError && m_lastError > 0) {
//     m_reverse = true;
//   }

//   //If in reverse return error as a negative
//   if (m_reverse) {
//     m_lastError = -error;
//     return -error;
//   } else {
//     m_lastError = error;
//     return error;
//   }
// }