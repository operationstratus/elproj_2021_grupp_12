
#ifndef ReadSD_h
#define ReadSD_h

#include "Arduino.h"


/////////////////////////////////////////// libraries for ReadSD
#include <SPI.h>
#include <SD.h>
#include <DS1307RTC.h>


//files
String alarmFile = "alarms.txt";




class ReadSD
{
  public:
    ReadSD(int stepsPerRevolution, int stepDelay);
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
