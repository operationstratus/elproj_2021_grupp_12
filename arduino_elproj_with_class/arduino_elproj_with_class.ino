//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////      LIBRARIES      ///////////////////////////////

#include "ReadSD.h"
ReadSD readSD(10); //const int chipSelect = 10;

#include "MenuLCD.h"
MenuLCD menuLCD(A0);


// ALARM
String nextAlarmTime = "";
String nextAlarmContent = "";


//-------------------------READING KEYBOARD
//const byte kbdIn = 0; // pin for the keyboard
//String returnStr = ""; // variable used inside readKBD() function
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

// SCREEN SAVE
int counter = 0;

// SOUND TOGGLE
byte soundOn = 0;


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////   GLOBAL VARIABLES  ///////////////////////////////



const byte buzzerPin = 10;



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////        SETUP        ///////////////////////////////
void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

  pinMode(buzzerPin, OUTPUT); 
  digitalWrite(buzzerPin, 0);

  menuLCD.printLCD("  WELCOME TO", "ROSETTEN");

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////        LOOP         ///////////////////////////////
void loop() {
  digitalWrite(buzzerPin, soundOn);


  /////////////////////////////////////////
  /////////////////////   MENU     ////////
  keyState = menuLCD.readKBD();

  if (counter % 4000 == 0) {
    //String line1 = "Next alarm " + nextAlarmTime;
    //menuLCD.printLCD(readSD.getTime(), line1);

    String line1 = "Next alarm: 17:23";
    //menuLCD.printLCD(readSD.getTime(), line1);
    menuLCD.printLCD("hej", "kth skrap");
    curMenuItem = 0;
  }
  counter++;
  
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////     FUNCTIONS       ///////////////////////////////
