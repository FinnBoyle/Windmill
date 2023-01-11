#include <PID_v1.h>
class OrientationPID {
private:
  double m_setpoint, m_input, m_output;
  int m_kp, m_ki, m_kd;
  PID m_pid;
public:
  OrientationPID(double setpoint, int kp, int ki, int kd);
  void setConstants(int kp, int ki, int kd);
  int rotationChange();
};