#include <Stepper.h>
#include "orientation_pid.h"
enum StepperState { OFF,
                    CLOCKWISE,
                    ANTI_CLOCKWISE,
                    PID };

class OrientationStepper {

private:
  StepperState m_state;
  OrientationPID* m_pid;
  Stepper* m_stepper;
public:
  OrientationStepper(Stepper* stepper, OrientationPID* pid);
  void handleState();
  void handleSteps();
  void update();
  StepperState getState();
  void setState(int state);
};