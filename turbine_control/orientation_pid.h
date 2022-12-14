class OrientationPID {
private:
  double m_setpoint, m_input, m_output;
  int m_kp, m_ki, m_kd;
public:
  OrientationPID(double setpoint, double input, double output, int kp, int ki, int kd);
  void setConstants(int kp, int ki, int kd);
};