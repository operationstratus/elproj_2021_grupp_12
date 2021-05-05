#include <LiquidCrystal.h>

#include "Arduino.h"
#include "MenuLCD.h"


//-------------------------READING KEYBOARD
const int kbdPin = A0;
int kbdIn = 0; // pin for the keyboard
String returnStr = ""; // variable used inside readKBD() function
String keyState = ""; // current key pressed: "E", "U", "D", "R", "L", or "N"
String prevKeyState = ""; // key pressed in the last loop cycle

//-------------------------CALLING LCD PRINT
String line0 = ""; // string to be printed on the first row on the lcd screen
String line1 = ""; // string to be printed on the second row on the lcd screen

//-------------------------MENU SYSTEM
int menuIntro = 0; // inital switch state of menuMain
int curMenuArray = 0;
int curMenuItem = 0;
String menuMainString[] = {"List alarms", "Set time", "Reset wheel", "Sound", "Shamoun!", "GK elak"};
String menuAlarmString[] = {"08:00", "12:30", "15:00", "18:45"};
String menuSoundString[] = {"Sound off", "Sound on"};
int leng = 0;


MenuLCD::MenuLCD(int in0, int in1, int in2, int in3, int in4, int in5, int kbdPin)
{ 
  lcd.begin(16,2);
  leng = sizeof(menuMainString)/sizeof(menuMainString[0]);
  menuWrite(menuMainString);

  in0 _in0;
  in1 _in1;
  in2 _in2;
  in3 _in3;
  in4 _in4;
  in5 _in5;
  kbdPin _kbdPin;


}

void MenuLCD::updateMenu() {
  ///////////////////////////////////////////////////// LOOP MENU AND KBD
  readKBD(); // updates the keyState value
  
  if (counter % 4000 == 0) {
    String line1 = "Next alarm " + nextAlarmTime;
    myLCDprint(getTime(), line1);
    curMenuItem = 0;
  }
  //Serial.println("curMenuArray = "+ String(curMenuArray));

  switch(curMenuArray) {
    case 0:
    if (keyState != prevKeyState && keyState != "N"){
      mainMenu();
    }
    break;
    case 1:
    if (keyState != prevKeyState && keyState != "N") {
      alarmMenu();
    }
    break;
    case 2:
    if (keyState != prevKeyState && keyState != "N") {
      soundMenu();
    }
    break;
  }
  prevKeyState = keyState;
  counter ++;
}

void MenuLCD::readKBD(){
  kbdIn = analogRead(kbdPin);
  // Serial.println(kbdIn);
  // Configuration:
  //               Up
  // Enter    Left    Right
  //              Down
  // -------------------------- 
  // Analog value of output:
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
  keyState = returnStr;
}

void MenuLCD::myLCDprint(String line0, String line1) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
}

void MenuLCD::menuWrite(String menu[]) {
  //--------------------------------WRITE THE MENUE
  //Serial.println(curMenuItem);
  line0 = ">" + menu[0+curMenuItem];
  if (curMenuItem < leng-1){
    line1 = " " + menu[1+curMenuItem];
  } else {
    line1 = " ";
  }

  //Serial.println("line0 = "+line0+"\nline1="+line1+"curMenuItem = "+curMenuItem+"\n\n");
  myLCDprint(line0, line1);
}


void MenuLCD::mainMenu() {
  menuWrite(menuMainString);
    counter = 0;
    // calculate the lenght of the current menu array
    leng = sizeof(menuMainString)/sizeof(menuMainString[0]);
    
    //Serial.println(keyState);
    if (keyState == "U" && curMenuItem > 0) {
      curMenuItem -= 1;
      menuWrite(menuMainString);
      Serial.println("pressed: "+keyState);
    } else if (keyState == "D" && curMenuItem < leng-1) {
      curMenuItem += 1;
      //Serial.println(curMenuItem);
      menuWrite(menuMainString);
      Serial.println("pressed: "+keyState);
    } else if (keyState == "R") {
      Serial.println("pressed: "+keyState);
      curMenuArray = curMenuItem+1;
      curMenuItem = 0;
      keyState = "N";
      updateMenu();
    }
}


void MenuLCD::alarmMenu() {
  counter = 0;
  menuWrite(menuAlarmString);
  // calculate the lenght of the current menu array
  leng = sizeof(menuAlarmString)/sizeof(menuAlarmString[0]);
  
  //Serial.println(keyState);
  if (keyState == "U" && curMenuItem > 0) {
    curMenuItem -= 1;
    menuWrite(menuAlarmString);
  }
  if (keyState == "D" && curMenuItem < leng-1) {
    curMenuItem += 1;
    menuWrite(menuAlarmString);
    Serial.println("pressed: "+keyState);
  }
  if (keyState == "E") {
    Serial.println("pressed: "+keyState);
    curMenuArray = 0;
    curMenuItem = 0;
    updateMenu();
    exit;
  }
  if (keyState == "L") {
    Serial.println("pressed: "+keyState);
    curMenuArray = 0;
    curMenuItem = 0;
    updateMenu();
    exit;
  }
}


void MenuLCD::soundMenu() {
  
  Serial.println("NUUUUUUU IIIIIII SOOOOOUUUND, keystate="+keyState);

  counter = 0;
  menuWrite(menuSoundString);
  // calculate the lenght of the current menu array
  leng = sizeof(menuSoundString)/sizeof(menuSoundString[0]);
  
  //Serial.println(keyState);
  if (keyState == "U" && curMenuItem > 0) {
    curMenuItem -= 1;
    menuWrite(menuSoundString);
  } else if (keyState == "D" && curMenuItem < leng-1) {
    curMenuItem += 1;
    menuWrite(menuSoundString);
    Serial.println("pressed: "+keyState);
  } else if (keyState == "E") {
    Serial.println("pressed: "+keyState);
    soundOn = curMenuItem;
    Serial.println("soundOn = "+String(soundOn));
    curMenuArray = 0;
    curMenuItem = 0;
    updateMenu();
    exit;
  } else if (keyState == "L") {
    Serial.println("pressed: "+keyState);
    curMenuArray = 0;
    curMenuItem = 0;
    updateMenu();
    exit;
  }
}