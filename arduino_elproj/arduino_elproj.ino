#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2); // generates an instance in the lcd

const int switchPin = 13;
const int kbdPin = A0;

int kbdIn = 0; // pin for the keyboard


String keyState = ""; // current key pressed: "E", "U", "D", "R", "L", or "N"
String prevKeyState = ""; // key pressed in the last loop cycle

String line0 = ""; // string to be printed on the first row on the lcd screen
String line1 = ""; // string to be printed on the second row on the lcd screen

int menueMain = 0; // inital switch state of menueMain


// Variabler som vi får tag på något sätt
String nextTime = "12:40"; // från en klocka?

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);

  
  // initial screen print
  line0 = " Next:  " + nextTime;
  line1 = ">Menue";
  myLCDprint(line0, line1);
}

void loop() {
  //read the current pressed keyboard
  keyState = keyPressed();
  
  if (keyState != prevKeyState) {
    if (keyState != "N") {
      lcd.clear();

      
      
      switch(menueMain){
        case 0:
        line0 = "Next:  " + nextTime;
        line1 = "Menue";
        myLCDprint(line0, line1);
        if (keyPressed() == "E") {
          menueMain = 1;
        }
        break;

        case 1:
        line0 = "My times";
        line1 = "Reset wheel";
        myLCDprint(line0, line1);
        if (keyPressed() == "L") {
          menueMain = 0;
        }    
        break;

      }
    }
  }  
  prevKeyState = keyState;
}

void myLCDprint(String line0, String line1) {
  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
}

String keyPressed() {
  kbdIn = analogRead(kbdPin);
  //Serial.println(kbdIn);
  /* Configuration:
   *               Up
   * Enter    Left    Right
   *              Down
   * -------------------------- 
   * Analog value of output:
   * None: 1023
   * Enter: 350-351
   * Left: 165-166
   * Right: 0
   * Up: 87-88
   * Down: 30-31
  */

  if (349 < kbdIn && kbdIn < 352) {
    return "E";
  }
  if (164 < kbdIn && kbdIn < 167) {
    return "L";
  }
  if (0 == kbdIn) {
    return "R";
  }
  if (86 < kbdIn && kbdIn < 89) {
    return "U";
  }
  if (29 < kbdIn && kbdIn < 32) {
    return "D";
  }
  return "N";
}
