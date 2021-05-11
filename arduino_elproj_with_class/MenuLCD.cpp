


#include "Arduino.h"

#include "MenuLCD.h"



#include <LiquidCrystal.h>
LiquidCrystal lcd(9,8,7,6,5,4);





MenuLCD::MenuLCD(int kbdPin)
{
  _kbdPin = kbdPin;
  lcd.begin(16,2); // 16 chars x 2 rows

}

String MenuLCD::readKBD(){
  String returnStr = ""; // variable used inside readKBD() function
  int kbdIn = analogRead(_kbdPin);
  // Serial.println(kbdIn);
  
  // Configuration:
  //               Up
  // Enter    Left    Right
  //              Down
  // -------------------------- 
  // Measuered analogRead value of output:
  // None: 1023
  // Enter: 345-360
  // Left: 165-175
  // Right: 0-10
  // Up: 85-95
  // Down: 25-40
  
  if (345 < kbdIn && kbdIn < 360) {
    returnStr = "E";
  } else if (164 < kbdIn && kbdIn < 175) {
    returnStr = "L";
  } else if (0 < kbdIn && kbdIn < 10) {
    returnStr = "R";
  } else if (85 < kbdIn && kbdIn < 95) {
    returnStr = "U";
  } else if (25 < kbdIn && kbdIn < 40) {
    returnStr = "D";
  } else if (kbdIn == 1023) {
    returnStr = "N";
  }
  return returnStr;
}

void MenuLCD::printLCD(String _line0, String _line1){
  lcd.setCursor(0,0);
  lcd.print(_line0);
  lcd.setCursor(0,1);
  lcd.print(_line1);
}

void MenuLCD::menuWrite(String menu[], int curMenuItem, int lengMenu) {
  String line0 = ">" + menu[0+curMenuItem];
  String line1 = " ";
  if (curMenuItem < lengMenu-1){
    line1 = " " + menu[1+curMenuItem];
  }
  printLCD(line0, line1);
}
