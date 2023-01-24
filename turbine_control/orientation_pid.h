#include <PID_v2.h>

class OrientationPID {
private:
  double m_setpoint, m_kp, m_ki, m_kd;
  PID_v2 m_pid;
 
public:
  OrientationPID(double setpoint, double kp, double ki, double kd, );
  void setConstants(double kp, double ki, double kd);
  double rotationChange(double input);


};