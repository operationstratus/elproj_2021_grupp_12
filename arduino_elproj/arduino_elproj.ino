#include <LiquidCrystal.h>

LiquidCrystal lcd(9,8,7,6,5,4); // generates an instance in the lcd
//LiquidCrystal lcd(12,11,5,4,3,2); // ORIGINAL CALL OF THE CLASS

//-------------------------READING KEYBOARD
const int kbdPin = A0;
int kbdIn = 0; // pin for the keyboard
String returnStr = ""; // variable inside readKBD() function
String keyState = ""; // current key pressed: "E", "U", "D", "R", "L", or "N"
String prevKeyState = ""; // key pressed in the last loop cycle

//-------------------------CALLING LCD PRINT
String line0 = ""; // string to be printed on the first row on the lcd screen
String line1 = ""; // string to be printed on the second row on the lcd screen

//-------------------------MENU SYSTEM
int menuIntro = 0; // inital switch state of menuMain
int menuMain = 0;
int curMenuItem = 0;
String menuMainString[] = {"List alarms", "Set time", "Reset wheel", "Sound"};
String menuListAlarms[] = {"08:00", "12:30", "15:00", "18:45"};
int leng = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);

  // calculate the lenght of the current menu array and write the initial menu
  leng = sizeof(menuMainString)/sizeof(menuMainString[0]);
  menuWrite(menuMainString);

}

void loop() {

  readKBD(); // updates the keyState value
  
  
  
  if (keyState != prevKeyState && keyState != "N") {
    Serial.println(keyState);

    // calculate the lenght of the current menu array
    leng = sizeof(menuMainString)/sizeof(menuMainString[0]);
    
    //Serial.println(keyState);
    if (keyState == "U" && curMenuItem > 0) {
      curMenuItem -= 1;
      menuWrite(menuMainString);
    }
    if (keyState == "D" && curMenuItem < leng-1) {
      curMenuItem += 1;
      //Serial.println(curMenuItem);
      menuWrite(menuMainString);
    }
    
  }
  prevKeyState = keyState;
}



void menuWrite(String menu[]) {
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

void myLCDprint(String line0, String line1) {
  // ------------------------------------------------PRINT TO LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
}

void readKBD() {
  // ----------------------------------------READ KEYBOARD
  kbdIn = analogRead(kbdPin);
  Serial.println(kbdIn);
  /* Configuration:
   *               Up
   * Enter    Left    Right
   *              Down
   * -------------------------- 
   * Analog value of output:
   * None: 1023
   * Enter: 345-360
   * Left: 165-175
   * Right: 0-10
   * Up: 85-95
   * Down: 25-40
  */
  

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
