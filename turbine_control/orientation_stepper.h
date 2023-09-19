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
  //"AUTO" function testing below
  AUTO_NO_HELP = 8,
  AUTO_FOUNDATIONS = 9,
  AUTO_FINISHINGS = 10,
  AUTO_FULL = 11,
  AUTO_CONCERT = 12,
  //"SEEK" function testing below
  SEEK_NO_HELP = 13,
  SEEK_FOUNDATIONS = 14,
  SEEK_FINISHINGS = 15,
  SEEK_FULL = 16,
  SEEK_CONCERT = 17
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

  //AUTO_FOUNDATIONS
  int currentRotation, optimalRotation;
  double* recordedPower;
  int currentIndex;
 
  //AUTO_FINISHINGS
  double* detectedVoltages;
  double* rotations;
  double totalRotation;
  double maxRotation;
  double rotationStep;
  int stepCount;

  //AUTO_FULL
  double bestRotation;
  double maxVoltage;
  double rotation_step;
  double currRotation;

  //AUTO_CONCERT
  double rotorAngle;
  double rotorSteps;
  double* voltageData;

  //SEEK_NO_HELP
  double stepAmount;
  bool clockwise;

  //SEEK_FOUNDATIONS
  double foundMaxRotation;
  double foundWindThreshold;
  double foundRotationAmount;
  double foundCurrentRotation;
  bool directionIsPositive;

  //SEEK_FINISHINGS
  int stepSize;
  int stepDirection;
  int stepIncrement;

  //SEEK_FULL
  double fullRotationAngle;
  bool fullDirection;

  //SEEK_CONCERT
  double concertMaxStep;
  double concertIncreaseFactor;
  double concertCurrentStep;
  bool concertRotateForward;

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