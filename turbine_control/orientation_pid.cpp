#include "orientation_pid.h"
#include <math.h>
#include <Arduino.h>

OrientationPID::OrientationPID(double setpoint, double kp, double ki, double kd, double reversalTheshold, int errorBufferSize, int movingAvgBufferSize)
  : m_pid(kp, ki, kd, PID::Direct) {
  // OrientationPID::OrientationPID(double setpoint, double kp, double ki, double kd) : m_pid(kp, ki, kd, PID::Direct) {
  m_setpoint = setpoint;
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
  m_pid.Start(0, 0, setpoint);
  m_reversalTheshold = reversalTheshold;
  m_reverse = false;  //Start going in a clockwise direction

  //buffer variables
  m_errorBufferIndex = 0;
  m_errorBufferLen = 0;
  m_errorBufferSize = errorBufferSize;
  m_movingAvgBufferSize = movingAvgBufferSize;
  m_errorBuffer = new double[m_errorBufferSize];
  m_movingAvgBuffer = new double[m_movingAvgBufferSize];

  //moving avg calculations
  m_errorTotal = 0;
  m_avgError = 0;
  m_AvgErrorGradient = 0;
}


void OrientationPID::setConstants(double kp, double ki, double kd) {
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
  m_pid.SetTunings(kp, ki, kd);
}

void OrientationPID::calculateAvgError(double currentError) {
  double oldValue = m_errorBuffer[m_errorBufferIndex];
  m_errorBuffer[m_errorBufferIndex] = currentError;
  m_errorBufferIndex = (m_errorBufferIndex + 1) % m_errorBufferSize;
  if (m_errorBufferLen < m_errorBufferSize) {
    m_errorBufferLen++;
  }

  m_errorTotal -= oldValue;
  m_errorTotal += currentError;

  m_avgError = m_errorTotal / m_errorBufferLen;
}
//(f(x) - f(x+h))/h approximation
// where f(x) is the oldest value in buffer, f(x+h) is the current value
// and h is the buffer length (number of time steps known)
void OrientationPID::calculateGradient() {
  double oldValue = 0;
  //if full take replaced value for f(x)
  if (m_movingAvgBufferLen == m_movingAvgBufferSize) {
    oldValue = m_movingAvgBuffer[m_movingAvgBufferIndex];
   
  } else {  //otherwise take index 0 for f(x)
    oldValue = m_movingAvgBuffer[0];
  }

  m_movingAvgBuffer[m_movingAvgBufferIndex] = m_avgError;
  m_movingAvgBufferIndex = (m_movingAvgBufferIndex + 1) % m_movingAvgBufferSize;

  if (m_movingAvgBufferLen < m_movingAvgBufferSize) {
    m_movingAvgBufferLen++;
  }

  //if only one value recorded return 0
  if (m_movingAvgBufferLen < 2) {
    m_AvgErrorGradient = 0;
    
  } else { //otherwise calculate gradient based off earliest known value
    m_AvgErrorGradient = (oldValue - m_avgError) / m_movingAvgBufferLen;
  }
}

double OrientationPID::rotationChange(double input) {
  double error = m_pid.Run(input);
  this->calculateAvgError(error);
  this->calculateGradient();
  Serial.print("Moving Avg Error: ");
  Serial.print(m_avgError);
  Serial.print("Gradient: ");
  Serial.println(m_AvgErrorGradient);


  //if error is increasing by more than the reversal theshold then change direction
  if (m_AvgErrorGradient < 0) {
    m_reverse = !m_reverse;
    Serial.println("REVERSED");
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