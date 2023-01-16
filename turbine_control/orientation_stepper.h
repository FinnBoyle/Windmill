#include <Stepper.h>
#include "orientation_pid.h"
enum StepperState { OFF = 0,
                    CLOCKWISE_AUTO = 1,
                    ANTI_CLOCKWISE_AUTO = 2,
                    PID_FAKE = 3,
                    PID = 4,
                    CLOCKWISE_STEPS = 5,
                    ANTI_CLOCKWISE_STEPS = 6,
};

class OrientationStepper {

private:
  StepperState m_state;
  OrientationPID* m_pid;
  Stepper* m_stepper;
  int m_rpm, m_steps, m_fakeWindDir;
  double m_rotation;
  unsigned long m_lastPIDUpdate = 0;
  
public:
  OrientationStepper(Stepper* stepper, OrientationPID* pid);
  void handleState();
  void handleSteps();
  void update();
  StepperState getState();
  void setState(int state);
  void setRPM(int rpm);
  void addSteps(int steps);
  double fakeVoltage();
  void rotate(int change);
};