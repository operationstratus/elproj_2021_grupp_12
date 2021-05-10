
#ifndef ReadSD_h
#define ReadSD_h

#include "Arduino.h"







class ReadSD
{
  public:
    ReadSD(int chipSelect);
    String readFromSD(String);
    String getNextAlarm();

    String getTime();
    bool setRTC(int, int);
    bool pollRTC();

    //getters.
    String getNextAlarmTime();
    String getNextAlarmContent();
    
  private:
    int _chipSelect;
};

#endif
