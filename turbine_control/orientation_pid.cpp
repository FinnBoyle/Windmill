#include "orientation_pid.h"
#include <math.h>
#include <Arduino.h>

OrientationPID::OrientationPID(double setpoint, double kp, double ki, double kd, int errorBufferSize, int movingAvgBufferSize)
  : m_pid(kp, ki, kd, PID::Direct) {
  m_setpoint = setpoint;
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
  m_pid.Start(0, 0, setpoint);
  m_rotation = 0;
  m_reverse = false;  //Start going in a clockwise direction
  m_errorUptrend = false;
  m_uptrendCount = 0;
  downtrendCount = 0;
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
  m_avgErrorGradient = 0;
  m_prevAvgErrorGradient = 0;

  //Oscillation calculations
  m_oscillationBufferSize = 5;
  m_optimalDirection = 0;
  m_optimalDirectionFound = false;
  m_optimalChangeSent = false;
  m_leftPositions = new double[m_oscillationBufferSize];
  m_rightPositions = new double[m_oscillationBufferSize];
  m_leftPositionsLen = 0;
  m_leftPositionsIndex = 0;
  m_rightPositionsLen = 0;
  m_rightPositionsIndex = 0;
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
  //If there is more than 6 in the array start taking off the oldest value
  if (m_errorBufferLen == m_errorBufferSize) {
    m_errorTotal -= oldValue;
  }
  m_errorBufferIndex = (m_errorBufferIndex + 1) % m_errorBufferSize;
  if (m_errorBufferLen < m_errorBufferSize) {
    m_errorBufferLen++;
  }

  m_errorTotal += currentError;

  m_avgError = m_errorTotal / m_errorBufferLen;
}
//(f(x) - f(x+h))/h approximation
// where f(x) is the oldest value in buffer, f(x+h) is the current value
// and h is the buffer length (number of time steps known)
void OrientationPID::calculateGradient() {
  double oldValue = 0;
  //if full take replaced value for f(x)
  if (m_movingAvgBufferLen == m_movingAvgBufferSize - 1) {
    Serial.println("TEST");
    oldValue = m_movingAvgBuffer[m_movingAvgBufferIndex];
    //else if moving avg buffer is not empty take item at index 0 for oldValue
  } else if (m_movingAvgBufferLen > 0) {
    Serial.println("TEST1");
    oldValue = m_movingAvgBuffer[0];
  }

  m_movingAvgBuffer[m_movingAvgBufferIndex] = m_avgError;
  m_movingAvgBufferIndex = (m_movingAvgBufferIndex + 1) % m_movingAvgBufferSize;

  if (m_movingAvgBufferLen < m_movingAvgBufferSize - 1) {
    m_movingAvgBufferLen++;
  }
  m_prevAvgErrorGradient = m_avgErrorGradient;
  //if only one value recorded return 0
  if (m_movingAvgBufferLen < 2) {
    m_avgErrorGradient = 0;

  } else {  //otherwise calculate gradient based off earliest known value
    m_avgErrorGradient = (m_avgError - oldValue) / m_movingAvgBufferLen;
  }
  Serial.print("OldValue: ");
  Serial.print(oldValue);
  Serial.print(" OldValIndex: ");
  Serial.print(m_movingAvgBufferIndex);
  Serial.print(" Moving Avg Error: ");
  Serial.print(m_avgError);
  Serial.print(" Error Total:");
  Serial.print(m_errorTotal);
  Serial.print(" Gradient: ");
  Serial.print(m_avgErrorGradient);
  Serial.print(" Uptrend:");
  Serial.print(m_errorUptrend);
  Serial.print(" Count:");
  Serial.println(m_uptrendCount);
}

void OrientationPID::addOscillationPoint() {
  //if we are changing direction to left/anticlockwise then we are at the right side of the oscillation
  if (m_reverse) {
    m_rightPositions[m_rightPositionsIndex] = m_rotation;
    m_rightPositionsIndex = (m_rightPositionsIndex + 1) % m_oscillationBufferSize;
    if (m_rightPositionsLen < m_oscillationBufferSize) {
      m_rightPositionsLen++;
    }
    //if we are changing direction to right/clockwise then we are at the left side of the oscillation
  } else {
    m_leftPositions[m_leftPositionsIndex] = m_rotation;
    m_leftPositionsIndex = (m_leftPositionsIndex + 1) % m_oscillationBufferSize;
    if (m_leftPositionsLen < m_oscillationBufferSize) {
      m_leftPositionsLen++;
    }
  }

  //If all buffers are now full calculate optimal point
  if (m_leftPositionsLen == m_rightPositionsLen && m_rightPositionsLen == m_oscillationBufferSize) {
    calculateOptimalPoint();
  }
}

//When oscillation endpoint buffers are full calculate optimal direction
void OrientationPID::calculateOptimalPoint() {
  //if called while buffers are not full do nothing
  if (m_leftPositionsLen != m_rightPositionsLen || m_rightPositionsLen != m_oscillationBufferSize) {
    return;
  }

  //Calculate the average of the left side and right side points of the recent oscillations
  double totalLeft = 0;
  double totalRight = 0;
  for (int i = 0; i < m_oscillationBufferSize; i++) {
    totalLeft += m_leftPositions[i];
    totalRight += m_rightPositions[i];
    Serial.print(m_leftPositions[i]);
    Serial.print(":");
    Serial.println(m_rightPositions[i]);

  }

  double avgLeftPoint = totalLeft/m_oscillationBufferSize;
  double avgRightPoint = totalRight/m_oscillationBufferSize;
  Serial.print("avgLeft: ");
  Serial.print(avgLeftPoint);
  Serial.print(" avgRight: ");
  Serial.print(avgRightPoint);
  double difference = avgRightPoint-avgLeftPoint;
  //Calculate the center between these points
  double optimalDirection = avgLeftPoint+(difference/2);
  Serial.print(" optimalDir: ");
  Serial.println(optimalDirection);
  m_optimalDirection = optimalDirection;
  m_optimalDirectionFound = true;
}



double OrientationPID::compute(double current_voltage, double current_rotation) {
  m_rotation = current_rotation;
  double error = m_pid.Run(current_voltage);
  calculateAvgError(error);
  calculateGradient();

  //if optimal direciton has been found go straight to that location if not already there
  //but do not send this twice
  if (m_optimalDirectionFound && !m_optimalChangeSent) {
    Serial.print(" optimal: ");
    Serial.println(m_optimalDirection);
    m_optimalChangeSent = true;
    //REPLACE THIS THEN IT WILL BE WORKING 100% HOPEFULLY!!
    return m_optimalDirection-m_rotation;
  } else if (m_optimalDirectionFound && m_optimalChangeSent) {
    return 0;
  }

  //if error uptrend has started and avg gradient is still positive
  if (m_errorUptrend && m_avgErrorGradient > 0) {
    m_uptrendCount++;
    //if error has been on an upward trend for 5 consecutive PID ticks then reverse direction and reset counter
    if (m_uptrendCount == 3) {
      // m_uptrendCount = 0;
      m_reverse = !m_reverse;
      addOscillationPoint();
      Serial.println("REVERSED");
    }
  } 
  //If error goes from a downtrend to and uptrend set m_errorUptrend to true
  else if (m_prevAvgErrorGradient <= 0 && m_avgErrorGradient > 0) {
    m_errorUptrend = true;
    m_uptrendCount++;

    //else if gradient is trending downwards again set uptrend to false and reset counter
  } else if (m_avgErrorGradient <= 0) {
    m_errorUptrend = false;
    m_uptrendCount = 0;
  }

  //If in reverse return error as a negative
  if (m_reverse) {
    return -error;
  } else {
    return error;
  }
}
