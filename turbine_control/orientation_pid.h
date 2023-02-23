#include <PID_v2.h>

class OrientationPID {
private:
  //PID
  double m_setpoint, m_kp, m_ki, m_kd;
  //Rotation
  double m_rotation;
  //Direction Change
  double m_errorTotal, m_avgError, m_avgErrorGradient, m_prevAvgErrorGradient;
  int m_uptrendCount, downtrendCount;
  bool m_errorUptrend;
  //Error history buffer
  int m_errorBufferSize, m_errorBufferLen, m_errorBufferIndex;
  //Moving Avg history buffer
  int m_movingAvgBufferSize, m_movingAvgBufferLen, m_movingAvgBufferIndex;
  bool m_reverse;
  PID_v2 m_pid;
  double *m_errorBuffer;
  double *m_movingAvgBuffer;

  //Storing rotation positions of each side of the oscillations
  int m_oscillationBufferSize;

  //For left side of oscillation
  double *m_leftPositions;
  int m_leftPositionsLen, m_leftPositionsIndex;
  //For right side of oscillation
  double *m_rightPositions;
  int m_rightPositionsLen, m_rightPositionsIndex;
  //Optimal wind direction found from calculating the center of oscillations
  double m_optimalDirection, m_optimalChangeSent;
  //Whether optimal direction has been found
  bool m_optimalDirectionFound;



  double calculateAvg();
  void calculateAvgError(double error);
  void calculateGradient();
  void addOscillationPoint();
  void calculateOptimalPoint();


public:
  OrientationPID(double setpoint, double kp, double ki, double kd, int errorBufferSize, int movingAvgBufferSize);
  void setConstants(double kp, double ki, double kd);
  double compute(double current_voltage, double current_rotation);
};