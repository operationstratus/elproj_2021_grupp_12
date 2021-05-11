#ifndef MenuLCD_h
#define MenuLCD_h

#include "Arduino.h"

class MenuLCD
{
  public:
    MenuLCD(int kbdPin);
    String readKBD();
    void menuWrite(String*, int, int);
    void printLCD(String, String);



  private:
    int _in0;
    int _in1;
    int _in2;
    int _in3;
    int _in4;
    int _in5;
    int _kbdPin;
};

#endif

/*
LiquidCrystal lcd(9,8,7,6,5,4);

*/
