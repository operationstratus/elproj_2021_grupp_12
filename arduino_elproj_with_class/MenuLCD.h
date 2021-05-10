#ifndef MenuLCD_h
#define MenuLCD_h

#include "Arduino.h"

class MenuLCD
{
  public:
    MenuLCD(int in0, int in1, int in2, int in3, int in4, int in5, int kbdPin);
    void updateMenu(String, String);
    void readKBD();
    void myLCDprint();
    void menuWrite();
    void mainMenu();
    void alarmMenu();
    void soundMenu();

    //getters.
    int getSoundOn();



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
