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
  int m_rpm, m_steps, m_fakeWindDir, m_pidInterval, m_bufferSize;
  double m_rotation;
  unsigned long m_lastMove = 0;

  //Buffers to supply historical rotation and PID error data to PID feed
  float* m_pidErrorHistory;
  int* m_rotationHistory;
  int m_bufferIndex;


public:
  OrientationStepper(Stepper* stepper, OrientationPID* pid, int pidInterval, int bufferSize);
  void update(double volts);
  StepperState getState();
  void setState(int state);
  void setRPM(int rpm);
  void addSteps(int steps);
  double fakeVoltage();
  int calculateSteps(double change);

  //PID feed buffer functions
  void resetBuffers();
  bool bufferFull();
  float* getErrorHistory();
  int* getRotationHistory();
  int getInterval();
};