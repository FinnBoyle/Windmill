#include "orientation_stepper.h"
#include <Arduino.h>
#include <math.h>
#define PI 3.14159265
OrientationStepper::OrientationStepper(Stepper* stepper, OrientationPID* pid,  int pidInterval, int bufferSize, double rotationTolerance) {
  m_stepper = stepper;
  m_pid = pid;
  m_state = OFF;
  m_steps = 0;
  m_rpm = 20;
  m_fakeWindDir = 180;
  m_rotation = 0;
  m_stepper->setSpeed(m_rpm);
  m_lastMove = 0;
  m_rotationTolerance = rotationTolerance;
  m_pidInterval = pidInterval;
  m_bufferSize = bufferSize;
  m_pidErrorHistory = new float[m_bufferSize];
  m_rotationHistory = new float[m_bufferSize];
  m_bufferIndex = 0;

  //generated for MOVE_STEPS_AND_BACK
  m_moveBackSteps = 0;
  m_moveDirection = 0;
  
  //used for AUTO_NO_HELP
  rotationDegrees = 360;
  rotationAmount = 40;
  rotationIncrement = rotationDegrees / rotationAmount;
  rotationKeys = new double[(int)rotationIncrement];
  rotationVolts = new double[(int)rotationIncrement];
  maxVoltageRotation = 0;
  rotationCounter = 0;

  //used for AUTO_FOUNDATIONS
  currentRotation = 0; //current rotation angle
  optimalRotation = 0; //optimal rotation angle found
  recordedPower = new double[9]; //recorded power at each step in the 360
  currentIndex = 0; //to keep track of where to insert the next power value

  //used for AUTO_FINISHINGS
  detectedVoltages = new double[9];
  rotations = new double[9];
  totalRotation = 0.0;
  maxRotation = 360.0;
  rotationStep = 40.0;
  stepCount = 0;

  //AUTO_FULL
  bestRotation = 0.0;
  maxVoltage = -1.0;
  rotation_step = 40.0;
  currRotation = 0.0;

  //AUTO_CONCERT
  rotorAngle = 0.0;
  rotorSteps = 60.0;
  concert_counter = 0;
  voltageData = new double[int(360.0/rotorSteps)];
  rotationData = new double[int(360.0/rotorSteps)];

  //SEEK_NO_HELP
  stepAmount = 5.0;
  clockwise = true;

  //SEEK_FOUNDATIONS
  foundWindThreshold = 0.25; // some arbitrary value to detect strong wind
  foundRotationAmount = 5.0; // initial rotation amount
  foundCurrentRotation = 0.0;
  directionIsPositive = true;

  //SEEK_FINISHINGS
  stepSize = 10; // define the initial step size
  stepDirection = 1; // define the initial direction (1 for forward, -1 for reverse)
  stepIncrement = 10; // define step increment value

  //SEEK_FULL
  fullRotationAngle = 10.0; // starting angle
  fullDirection = true; // true for clockwise, false for counter-clockwise

  //SEEK_CONCERT
  concertIncreaseFactor = 5; // how much the step should increase after each full cycle (this value determines how quickly the oscillation goes)
  concertCurrentStep = 0; // current rotation step value (starting step)
  concertRotateForward = true; // direction of rotation (starting direction)
  concertThreshold = 0.25;
}

// double OrientationStepper::fakeVoltage() {
//   return 1.5 * exp(-0.0001 * pow((m_rotation - m_fakeWindDir), 2));
// }

double OrientationStepper::fakeVoltage() {
  return sin(m_rotation/57.3)/2.0;
}


double normalRange(double rotation) {
    double normalVal = fmod(rotation, 360);
    if (normalVal < 0) {
        normalVal += 360;
    }
    
    return normalVal;
}

void OrientationStepper::update(double volts) {
  if (m_state == CLOCKWISE_AUTO) {
    unsigned long now = millis();
    if (now - m_lastMove > 20) {
      m_lastMove = now;
      int steps = calculateSteps(20);
      m_stepper->step(steps);
    }
  } else if (m_state == ANTI_CLOCKWISE_AUTO) {
    unsigned long now = millis();
    if (now - m_lastMove > 50) {
      m_lastMove = now;
      int steps = calculateSteps(-20);
      m_stepper->step(steps);
    }
  }else if (m_state == PID_FAKE) {
    unsigned long now = millis();
    //every 2s send an initialization request or turbine metrics to ESP8266
    if (now - m_lastMove > m_pidInterval) {
      m_lastMove = now;
      double voltage = fakeVoltage();
      double change = m_pid->compute(voltage,m_rotation);
      // Serial.println(change);
      m_pidErrorHistory[m_bufferIndex] = change;
      //remove overflow from rotation threshold
      m_rotationHistory[m_bufferIndex] = normalRange(m_rotation);
      m_bufferIndex++;
      m_stepper->step(calculateSteps(change));
    }
  } else if (m_state == PID) {
    unsigned long now = millis();
    //every 2s send an initialization request or turbine metrics to ESP8266
    if (now - m_lastMove > m_pidInterval) {
      m_lastMove = now;
      double voltage = volts;
      double change = m_pid->compute(voltage,m_rotation);
      // Serial.println(change);
      m_pidErrorHistory[m_bufferIndex] = change;
      //remove overflow from rotation threshold
      m_rotationHistory[m_bufferIndex] = normalRange(m_rotation);
      m_bufferIndex++;
      m_stepper->step(calculateSteps(change));
    }
  } else if (m_state == CLOCKWISE_STEPS) {
    if (m_steps > 0) {
      m_stepper->step(calculateSteps(m_steps * 1.8));
      m_steps = 0;
    }
  } else if (m_state == ANTI_CLOCKWISE_STEPS) {
    if (m_steps > 0) {
      m_stepper->step(calculateSteps(-m_steps * 1.8));
      m_steps = 0;
    }
  } else if (m_state == MOVE_STEPS_AND_BACK) { //simple function generated by chatgpt
    if(m_steps > 0) {
      m_stepper->step(calculateSteps(m_steps * 1.8));
      delay(1000);
      m_stepper->step(calculateSteps(-m_steps * 1.8));
      m_steps = 0;
    }
  } else if (m_state == AUTO_NO_HELP) { //completely done by me
    unsigned long now = millis();
    int steps = 0;

    //continue to rotate until a 360 has been performed
    if (maxVoltageRotation < rotationDegrees) {
      if (now - m_lastMove > 2000) {
        m_lastMove = now;
        
        //capture the current voltage detected and rotation amount
        double voltage = volts;
        rotationKeys[rotationCounter] = maxVoltageRotation;
        rotationVolts[rotationCounter] = voltage;
        
        //rotate a specified distance
        steps = calculateSteps(rotationAmount);
        m_stepper->step(steps);

        //update variables for next increment
        maxVoltageRotation += rotationAmount;
        rotationCounter++;
      }
    } else if (maxVoltageRotation >= rotationDegrees) {
      //return to highest recorded voltage rotation
      delay(1000);
      double highestVoltage = 0;
      double returnRotation = 0;
      //discover maximum voltage detected in the full rotation
      //capture the location in the array of the maximum voltage detected
      for (int i = 0; i < rotationIncrement; i++) {
        if (rotationVolts[i] > highestVoltage) {
          //capture the highest recorded voltage, set return rotation
          highestVoltage = rotationVolts[i];
          returnRotation = rotationKeys[i];
        }
      }
      //Rotate to location of highest voltage generation
      steps = calculateSteps(-returnRotation);
      m_stepper->step(steps);

      //simulate real windmill operation (utilising the strongest wind source for real world work)
      delay(5000);

      //reset variables and start function again
      for (int i = 0; i < rotationIncrement; i++) {
        rotationKeys[i] = 0;
        rotationVolts[i] = 0;
      }
      maxVoltageRotation = 0;
      rotationCounter = 0;
    }
  } else if (m_state == AUTO_FOUNDATIONS) { //begun by ChatGPT, finished by me
    unsigned long now = millis();

    if (currentRotation >= 360) {
      //find optimal rotation angle after a 360
      double maxPower = recordedPower[0];
      int maxIndex = 0;
      for(int i = 0; i < 9; i++) {
        if(recordedPower[i] > maxPower) {
          maxPower = recordedPower[i];
          maxIndex = i;
        }
      }

      optimalRotation = maxIndex * 40; //40 degree increment

      //move to the optimal rotation
      currentRotation = optimalRotation;
      m_stepper->step(calculateSteps(-currentRotation));
      delay(5000);

      //clear recorded data and reset current rotation to start the process again
      for (int i = 0; i < 9; i++) {
        recordedPower[i] = 0;
      }
      currentIndex = 0;
      currentRotation = 0;
    } else {
      if (now - m_lastMove > 2000) {
        m_lastMove = now;
        //rotate the windmill by 40 degrees
        m_stepper->step(calculateSteps(40));
        currentRotation += 40;

        //record the power generation at this rotation angle
        double voltage = volts;
        if(currentIndex < 9) {
          currentIndex++;
          recordedPower[currentIndex] = volts;
        }
      }
    }
  } else if (m_state == AUTO_FINISHINGS) { //begun by me, finished by ChatGPT
    unsigned long now = millis();
    
    if(totalRotation < maxRotation) {
      if(now - m_lastMove > 2000) {
        m_lastMove = now;

        //store detected windmill generated voltage
        double voltage = volts;
        detectedVoltages[stepCount] = voltage;
        rotations[stepCount] = totalRotation;

        m_stepper->step(calculateSteps(rotationStep));

        totalRotation += rotationStep;
        stepCount++;
      }
    } else {
      //determine maximum voltage rotation
      double maxVoltage = detectedVoltages[0];
      int maxIndex = 0;
      for (int i = 1; i < stepCount; ++i) {
        if (detectedVoltages[i] > maxVoltage) {
          maxVoltage = detectedVoltages[i];
          maxIndex = i;
        }
      }
      double bestRotation = rotations[maxIndex];

      //rotate back to best voltage
      m_stepper->step(calculateSteps(-bestRotation));

      delay(5000);

      //clear variables
      for (int i = 0; i < 9; i++) {
        detectedVoltages[i] = 0.0;
        rotations[i] = 0.0;
      }
      totalRotation = 0.0;
      stepCount = 0;
      }
  } else if (m_state == AUTO_FULL) { //completely done by ChatGPT
    unsigned long now = millis();

    if (currRotation < 360.0) {
      if (now - m_lastMove > 2000) {
        m_lastMove = now;
        m_stepper->step(calculateSteps(rotation_step)); //40 degree steps

        double voltage = volts;

        if (voltage > maxVoltage) {
          maxVoltage = voltage;
          bestRotation = 360.0 - currRotation;
        }

        currRotation += rotation_step;
      }
    } else {
      //once optimal rotation found, maintain it
      m_stepper->step(calculateSteps(-bestRotation));

      delay(5000);

      //reset variables
      maxVoltage = 0.0;
      currRotation = 0.0;
      bestRotation = 0.0;
    }
  } else if (m_state == AUTO_CONCERT) {
    unsigned long now = millis();

    if (rotorAngle > 360.0) {  
      double maxVolts = voltageData[0]; //initialise with the first element
      double appropriateRotation = rotationData[0];
      for (int i = 0; i < concert_counter; i++) {
        if (voltageData[i] > maxVolts) {
          maxVolts = voltageData[i];
          appropriateRotation = rotationData[i];
        }
      }
      m_stepper->step(-appropriateRotation);

      delay(5000);

      for(int i = 0; i <= int(360.0/rotorSteps); i++) {
        voltageData[i] = 0.0;
        rotationData[i] = 0.0;
      }
      rotorAngle = 0.0;
      concert_counter = 0;
    
    } else {
      if (now - m_lastMove > 2000) {
        m_lastMove = now;

        double voltage = volts;

        m_stepper->step(calculateSteps(rotorSteps));

        voltageData[concert_counter] = voltage;
        rotationData[concert_counter] = rotorAngle;
        concert_counter++;

        rotorAngle += rotorSteps;
      }
    }    
  } else if (m_state == SEEK_NO_HELP) { // done by me
    unsigned long now = millis();
    double voltage = volts;

    if (stepAmount >= 360.0) {
      stepAmount -= 360.0;
    }

    if (now - m_lastMove > 200) {
      m_lastMove = now;
      if (voltage < 0.25) { //arbitrary voltage threshold
        if (clockwise) {
          m_stepper->step(stepAmount);
          clockwise = false;
        } else {
          m_stepper->step(-stepAmount);
          clockwise = true;
        }
        stepAmount += 5;
      } else {
        //strong wind source detected
        stepAmount = 5.0;
        delay(3000);
      }
    }
  } else if (m_state == SEEK_FOUNDATIONS) { // started by chatgpt, finished by me
    unsigned long now = millis();
    double voltage = volts; // get wind strength

    if (now - m_lastMove > 200) {
      if (voltage > foundWindThreshold) { // if strong enough wind is detected
        // reset variables
        foundCurrentRotation = 0.0;
        delay(3000);
      } else {
        if (directionIsPositive) {
          m_stepper->step(foundCurrentRotation);
        } else {
          m_stepper->step(-foundCurrentRotation);
        }

        foundCurrentRotation += foundRotationAmount;
        directionIsPositive = !directionIsPositive;
      }
    }
  } else if (m_state == SEEK_FINISHINGS) { // started by me, finished by chatgpt
    unsigned long now = millis();
    double voltage = volts;
    
    if (now - m_lastMove > 200) {
      m_lastMove = now;
      if (voltage < 0.25) {
        // rotate the stepper
        m_stepper->step(stepSize * stepDirection);

        //change direction
        stepDirection *= -1;
        // increase step size
        stepSize += stepIncrement;
      } else {
        delay(3000); // delay for 3 seconds

        // reset variables to their original state
        stepSize = 10;
        stepDirection = 1;
      }
    }
  } else if (m_state == SEEK_FULL) { // done entirely by chatgpt
    unsigned long now = millis();
    double voltage = volts;

    if (now - m_lastMove > 200) {
      // check wind voltage
      if (voltage > 0.25) {
        delay(3000);

        // reset variables
        fullRotationAngle = 10.0;
        fullDirection = true;
      } else {
        // rotate windmill
        m_stepper->step(fullDirection ? fullRotationAngle : -fullRotationAngle);

        // toggle direction and increase rotation angle for next call
        fullDirection = !fullDirection;
        fullRotationAngle += 10.0;
      }
    }
  } else if (m_state == SEEK_CONCERT) { // done by me, side by side with chatgpt
    unsigned long now = millis();
    double voltage = volts;

    if(now - m_lastMove > 200) {
      if (voltage > concertThreshold) {
        concertCurrentStep = 0.0;

        delay(5000);
      } else {
        if (concertRotateForward) {
          m_stepper->step(concertCurrentStep);
          concertCurrentStep += concertIncreaseFactor;
          concertRotateForward = false;
        } else {
          m_stepper->step(-concertCurrentStep);
          concertCurrentStep += concertIncreaseFactor;
          concertRotateForward = true;
        }
      }
    }
  }
}

StepperState OrientationStepper::getState() {
  return m_state;
}

void OrientationStepper::setState(int state) {
  m_state = state;
}

void OrientationStepper::setRPM(int rpm) {
  if (rpm != m_rpm) {
    m_rpm = rpm;

    m_stepper->setSpeed(m_rpm);
  }
}

void OrientationStepper::addSteps(int steps) {
  m_steps += steps;
}


int OrientationStepper::calculateSteps(double degrees) {
  int steps = 0;
  double old_rotation = m_rotation;
  // degrees = 50
  //If new rotation exceeds  (360 + m_rotationTolerance) degrees go the same position within 0-(360 + m_rotationTolerance) range
  if ((m_rotation + degrees) > (360 + m_rotationTolerance)) {
    double overshoot = m_rotation + degrees - 360;  //e.g. degrees over 360
    m_rotation = overshoot;                      //rotation is the the same rotation as just the degrees over 360
    steps -= (old_rotation - overshoot) / 1.8;
    //If new rotation is less than  (360 + m_rotationTolerance) degrees go the  same position within 0-(360 + m_rotationTolerance) range
  } else if ((m_rotation + degrees) < (0 - m_rotationTolerance)) {
    double overshoot = m_rotation + degrees;
    m_rotation =  360 + overshoot;
    steps += (m_rotation - old_rotation) / 1.8;
    //Otherwise move normally
  } else {
    steps = degrees / 1.8;
    m_rotation = m_rotation + degrees;
  }
  if (steps == 0 && degrees > 0) {
    m_rotation = m_rotation + 1.8;
    return 1;
  }
  return steps;
}

  double OrientationStepper::getRotation() {
    return m_rotation;
  }

 void OrientationStepper::resetBuffers() {
   m_bufferIndex = 0;
 }
  bool OrientationStepper::bufferFull() {
    return m_bufferIndex >= m_bufferSize;
  }
  float* OrientationStepper::getErrorHistory() {
    return m_pidErrorHistory;
  }
  float* OrientationStepper::getRotationHistory() {
    return m_rotationHistory;
  }
  int OrientationStepper::getInterval() {
    return m_pidInterval;
  }
