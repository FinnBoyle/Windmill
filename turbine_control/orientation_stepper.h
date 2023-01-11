#include <Stepper.h>
#include "orientation_pid.h"
enum StepperState { OFF,
                    CLOCKWISE_AUTO,
                    ANTI_CLOCKWISE_AUTO,
                    PID,
                    CLOCKWISE_STEPS,
                    ANTI_CLOCKWISE_STEPS, };

class OrientationStepper {

private:
  StepperState m_state;
  OrientationPID* m_pid;
  Stepper* m_stepper;
  int m_rpm, m_steps;
public:
  OrientationStepper(Stepper* stepper, OrientationPID* pid, int rpm);
  void handleState();
  void handleSteps();
  void update();
  StepperState getState();
  void setState(int state);
  void setRPM(int rpm);
  void addSteps(int steps);

};