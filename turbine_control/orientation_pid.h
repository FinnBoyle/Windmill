#include <PID_v2.h>

class OrientationPID {
private:
  double m_setpoint, m_kp, m_ki, m_kd, m_reversalTheshold, m_errorTotal, m_prevAvgError, m_AvgError;
  int m_bufferSize, m_bufferLen, m_bufferIndex;
  bool m_reverse;
  PID_v2 m_pid;
  double* m_errorBuffer;

  

  double calculateAvg();
  void calculateNextError(double error);

 
public:
  OrientationPID(double setpoint, double kp, double ki, double kd, double reversalTheshold, int bufferSize );
  void setConstants(double kp, double ki, double kd);
  double rotationChange(double input);
};