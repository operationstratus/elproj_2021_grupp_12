
#ifndef ReadSD_h
#define ReadSD_h

#include "Arduino.h"







class ReadSD
{
  public:
    ReadSD(int chipSelect);
    String readFromSD(String);
    String getNextTime();
    String getNextString();
    
  private:
    int _chipSelect;
};

#endif
