
#include "Arduino.h"
#include "StepperMotor.h"

#define dirPin 2
#define stepPin 3

StepperMotor::StepperMotor(int stepsPerRevolution, int stepDelay)
{
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  _stepsPerRevolution = stepsPerRevolution;
  _stepDelay = stepDelay;
}

void StepperMotor::dispence(){
  // Set the spinning direction clockwise:
  Serial.println("Dispencing");
  digitalWrite(dirPin, HIGH);
  // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < (_stepsPerRevolution)/8; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(_stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(_stepDelay);
  }
}
