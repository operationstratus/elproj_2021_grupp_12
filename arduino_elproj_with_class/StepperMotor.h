
#ifndef StepperMotor_h
#define StepperMotor_h

#include "Arduino.h"

class StepperMotor
{
  public:
    StepperMotor(int stepsPerRevolution, int stepDelay);
    void dispence();
  private:
    int _stepsPerRevolution;
    int _stepDelay;
};

#endif


/*
#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200*2 // times two since we have enabled half stepping
#define stepDelay 5000

*/