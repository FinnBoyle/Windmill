#include <PID_v2.h>

class OrientationPID {
private:
  double m_setpoint, m_kp, m_ki, m_kd;
  int m_pidInterval, errorMax;
  PID_v2 m_pid;
  // QueueArray <float> recentPidErrors;
public:
  OrientationPID(double setpoint, double kp, double ki, double kd);
  void setConstants(double kp, double ki, double kd);
  double rotationChange(double input);
  void clearErrors();
  // void errorsFull

};