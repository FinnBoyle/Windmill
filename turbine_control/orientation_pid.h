#include <PID_v2.h>

class OrientationPID {
private:
  //PID
  double m_setpoint, m_kp, m_ki, m_kd;
  //Direction Reversal
  double m_reversalTheshold, m_errorTotal, m_avgError, m_AvgErrorGradient;
  //Error history buffer
  int m_errorBufferSize, m_errorBufferLen, m_errorBufferIndex;
  //Moving Avg history buffer
  int m_movingAvgBufferSize, m_movingAvgBufferLen, m_movingAvgBufferIndex;
  bool m_reverse;
  PID_v2 m_pid;
  double *m_errorBuffer;
  double *m_movingAvgBuffer;



  double calculateAvg();
  void calculateAvgError(double error);
  void calculateGradient();


public:
  OrientationPID(double setpoint, double kp, double ki, double kd, double reversalTheshold, int errorBufferSize, int movingAvgBufferSize);
  void setConstants(double kp, double ki, double kd);
  double rotationChange(double input);
};