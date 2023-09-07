#include <Stepper.h>
#include "orientation_pid.h"

enum StepperState {
  OFF = 0,
  CLOCKWISE_AUTO = 1,
  ANTI_CLOCKWISE_AUTO = 2,
  PID_FAKE = 3,
  PID = 4,
  CLOCKWISE_STEPS = 5,
  ANTI_CLOCKWISE_STEPS = 6,
	MOVE_STEPS_AND_BACK = 7, //CHATGPT ADDITION
  //auto function testing below
  AUTO_NO_HELP = 8,
  AUTO_FOUNDATIONS = 9,
  AUTO_FINISHINGS = 10,
  AUTO_FULL = 11
};

class OrientationStepper {

private:
  StepperState m_state;
  OrientationPID* m_pid;
  Stepper* m_stepper;
  int m_rpm, m_steps, m_fakeWindDir, m_pidInterval, m_bufferSize;
  double m_rotation, m_rotationTolerance;
  unsigned long m_lastMove = 0;
  int m_moveBackSteps, m_moveDirection; //MOVE_STEPS_AND_BACK
  
  //AUTO_NO_HELP
  double* rotationKeys;
  double* rotationVolts;
  double maxVoltageRotation;
  int rotationCounter, rotationDegrees, rotationAmount;
  double rotationIncrement;
 
  //AUTO_FINISHINGS
  double* detectedVoltages;
  double* rotations;
  double totalRotation;
  double maxRotation;
  double rotationStep;
  int stepCount;

  //Buffers to supply historical rotation and PID error data to PID feed
  float* m_pidErrorHistory;
  float* m_rotationHistory;
  int m_bufferIndex;


public:
  OrientationStepper(Stepper* stepper, OrientationPID* pid, int pidInterval, int bufferSize, double rotationTolerance);
  void update(double volts);
  StepperState getState();
  void setState(int state);
  void setRPM(int rpm);
  void addSteps(int steps);
  double fakeVoltage();
  int calculateSteps(double change);
  double getRotation();
  void moveStepsAndBack();
  void setMoveBackSteps(int steps);

  //PID feed buffer functions
  void resetBuffers();
  bool bufferFull();
  float* getErrorHistory();
  float* getRotationHistory();
  int getInterval();
};