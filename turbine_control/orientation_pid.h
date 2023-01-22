#include <PID_v2.h>

class OrientationPID {
private:
  double m_setpoint, m_kp, m_ki, m_kd;
  int m_pidInterval, m_numErrors;
  PID_v2 m_pid;
  float* m_pidErrors;
public:
  OrientationPID(double setpoint, double kp, double ki, double kd, int pidInterval, int errorBufferSize);
  void setConstants(double kp, double ki, double kd);
  double rotationChange(double input);
  void clearBuffer();
  void bufferFull();
  float* getErrors();
  int getInterval();

};