//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////        LIBRARIES       ////////////////////////////////

#include <LiquidCrystal.h>
LiquidCrystal lcd(9,8,7,6,5,4); // generates an instance in the lcd

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////    GLOBAL VARIABLES    ////////////////////////////////


//-------------------------READING KEYBOARD
const byte kbdPin = A0;
char keyState = 'N'; // current key pressed: "E", "U", "D", "R", "L", or "N"
char prevKeyState = 'N'; // key pressed in the last loop cycle

//-------------------------CALLING LCD PRINT
String line0 = ""; // string to be printed on the first row on the lcd screen
String line1 = ""; // string to be printed on the second row on the lcd screen


//-------------------------MENU SYSTEM
int curMenuArray = 0; // keeps track of current menu
int curMenuItem = 0;  // keeps track for current item in the current menu
int changedMenu = 0;
String menuMainString[] = {"List alarms", "Set time", "Reset wheel", "Sound", "Shamoun!", "GK elak"};
String menuAlarmString[] = {"08:00", "12:30", "15:00", "18:45"};
String menuSoundString[] = {"Sound off", "Sound on"};
int menuLeng = 0;


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////////// CLOCK, SD READER, ALARM ///////////////////////////////

// include the SD library:
#include <SPI.h>
#include <SD.h>
//include RTC
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

//files
char alarmFile[] = "alarms.txt";

//variable wires
const int chipSelect = 10;

//variables
char nextAlarmTime[] = "13:37";
String nextAlarmContent = "";

//Buzzer
const int buzzerPin = 10;


// SCREEN SAVE
const int screenSaverTime = 100000;
int counter = screenSaverTime;


// ALARM
int soundOn = 0;


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////       STEPPER MOTOR     ///////////////////////////////

/*Example sketch to control a stepper motor with A4988/DRV8825 stepper motor driver and Arduino without a library. More info: https://www.makerguides.com */
// Define stepper motor connections and steps per revolution:
#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200*2 // times two since we have enabled half stepping
#define stepDelay 5000


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////        SETUP        ///////////////////////////////

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

  // INIT THE ALARM AND SD
  //getNextAlarm();
  //pinMode(buzzerPin, OUTPUT);
  //digitalWrite(buzzerPin, LOW);

  // INIT THE LCD AND MENU VARS
  lcd.begin(16,2);
  //menuLeng = sizeof(menuMainString)/sizeof(menuMainString[0]); // calculate the lenght of the current menu array and write the initial menu
  printLCD("   WELCOME TO", "    ROSETTEN");


  // INIT STEPPER MOTOR SYSTEM
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);


  //??? // TESTS
  //setRTC(17, 4);
  //getNextAlarm();
  //Alarm();

  
  delay(1500);
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////        LOOP         ///////////////////////////////
void loop() {
  digitalWrite(buzzerPin, soundOn);

  updateKBD();
  updateMenu();

  


  ///////////////////////////////////////////////////// UPDATES AT END OF LOOP
  counter++;
  prevKeyState = keyState;
  




  
  

  

  


  ///////////////////////////////////////////////////// LOOP SD AND ALARM
  //Serial.println("getTime: "+getTime()+" nextAlarmTime: "+nextAlarmTime);
  //if(getTime() == nextAlarmTime) alarm();
  
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////      FUNCTIONS      ///////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//////////////////////////      FUNC MENU     ///////////////////////////////

void menuWrite(String menu[]) {

  line0 = ">" + menu[0 + curMenuItem];
  if (curMenuItem < menuLeng-1){
    line1 = " " + menu[1+curMenuItem];
  } else {
    line1 = " ";
  }
  printLCD(line0, line1);
}

void printLCD(String Line0, String Line1) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(Line0);
  lcd.setCursor(0,1);
  lcd.print(Line1);
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////      FUNC KBD      ///////////////////////////////

void updateKBD() {
  // updates keyState
  
  int kbdIn = analogRead(kbdPin);
  //Serial.println("kbdIn = "+String(kbdIn)); //debug
  
            //Configuration:
            //               Up
            // Enter    Left    Right
            //              Down
            // -------------------------- 
            // Analog value of output:
            // None: 1023
            // Enter: 345-360
            // Left: 160-175
            // Right: 0-10
            // Up: 80-95
            // Down: 25-40
  
  if (345 < kbdIn && kbdIn < 360) {
    keyState = 'E';
  } else if (160 < kbdIn && kbdIn < 175) {
    keyState = 'L';
  } else if (0 <= kbdIn && kbdIn < 10) {
    keyState = 'R';
  } else if (80 < kbdIn && kbdIn < 95) {
    keyState = 'U';
  } else if (25 < kbdIn && kbdIn < 40) {
    keyState = 'D';
  } else if (kbdIn == 1023) {
    keyState = 'N';
  }
  delay(100);
}


void updateMenu(){
  ///////////////////////////////////////////////////////////////// LOOP MENU
  //---------------------------------------SCREEN SAVER
  if (counter % screenSaverTime == 0) {
    //String line1 = "Next alarm " + nextAlarmTime;
    //myLCDprint(getTime(), line1);
    printLCD("     13:37", "Next Alarm 69:69");
    curMenuItem = 0;
  }

  
  if (changedMenu or (keyState != prevKeyState and keyState != 'N')){
    changedMenu = 0;

    Serial.println("pressed: "+String(keyState));

    counter = 0; // reset screen time counter
    switch(curMenuArray) {
      case 0:
        menuLeng = sizeof(menuMainString)/sizeof(menuMainString[0]); // calculate the length of a menuArray
        if (keyState == 'U' && curMenuItem > 0) {
          curMenuItem--;
          menuWrite(menuMainString);
        } else if (keyState == 'D' && curMenuItem < menuLeng-1){
          curMenuItem++;
          menuWrite(menuMainString);
        } else if (keyState == 'R'){
          Serial.println("Entered "+String(menuMainString[curMenuItem]));
          curMenuArray = curMenuItem+1;
          curMenuItem = 0;
          changedMenu = 1;
        } else {
          menuWrite(menuMainString);
        }
      break;
      
      case 1:
        menuLeng = sizeof(menuAlarmString)/sizeof(menuAlarmString[0]); // calculate the length of a menuArray
        if (keyState == 'U' && curMenuItem > 0) {
          curMenuItem--;
          menuWrite(menuAlarmString);
        } else if (keyState == 'D' && curMenuItem < menuLeng-1){
          curMenuItem++;
          menuWrite(menuAlarmString);
        } else if (keyState == 'L'){
          curMenuArray = 0;
          curMenuItem = 0;
          changedMenu = 1;
        } else {
          menuWrite(menuAlarmString);
        }
      break;

      case 2:
        menuLeng = sizeof(menuSoundString)/sizeof(menuSoundString[0]); // calculate the length of a menuArray
        if (keyState == 'U' && curMenuItem > 0) {
          curMenuItem--;
          menuWrite(menuSoundString);
        } else if (keyState == 'D' && curMenuItem < menuLeng-1){
          curMenuItem++;
          menuWrite(menuSoundString);
        } else if (keyState == 'E'){
          curMenuArray = 0;
          soundOn = curMenuItem;
          Serial.println("soundOn = "+String(soundOn));
          curMenuItem = 0;
          changedMenu = 1;
        } else {
          menuWrite(menuSoundString);
        }
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/////////////////////// FUNCTIONS FOR SD, ALARM, CLOCK/////////////////////////////


//checks all alarms on SD card and selects the next alarm to be executed
//nextAlarmTime is set as next alarm and accompanying string is set in nextAlarmContent
void getNextAlarm(String alarmString){
  String tempTime = "";
  nextAlarmTime = "23:59";
  Serial.println(alarmString);
  for(int i = 0; i < alarmString.length(); i++ ) {
     if(alarmString[i] == '?'){
      //compare times
      tempTime.trim();
      Serial.println(tempTime+" "+getTime()+" "+nextAlarmTime);
      if(tempTime > getTime() && tempTime < nextAlarmTime){ //"smallest" time AFTER current time
        nextAlarmTime = tempTime;
        nextAlarmContent = alarmString.substring(i+1,alarmString.substring(i+1).indexOf(';')+i+1);
      }
     }
     else if(alarmString[i] == ';'){
      tempTime = "";
     }
     else {
      tempTime += alarmString[i];
     }
  }
  //Serial.println("#"+nextAlarmTime+"#");
  //Serial.println("#"+nextAlarmContent+"#");
}
/* //denna ska fixas
string getAlarmInfo(char alarmfile[]){
  String alarmString = readFromSD(alarmFile);
  byte nrOfLines = 0;
  for(int i = 0; i < alarmString.length(); i++ ) {
    if(alarmString[i] == ';'){
      nrOfLines ++;
    }
  }
  char *pointer[nrOfLines];
  int index = 0;
  String temp;
  for(int i = 0; i < nrOfLines; i++ ) {
    //set pointer of line i to content of line i in format 'hh:MM?ttt...t;'
    *pointer[i] = alarmString.subString(index, alarmString.substring(index+1).indexOf(';'));
  }
  for(int i = 0; i < nrOfLines; i++ ) {
    Serial.println(*pointer[i]);
  }
}
*/
//activates the alarm and then calls getNextAlarm() after alarm has been deactivated
void alarm(){
  Serial.println(String(F("Alarm!")));
  if(nextAlarmContent == '1'){
    dispense();
  } else {
    myLCDprint(String(F("Take your meds!")), "");
    //myLCDprint(nextAlarmContent, "");
  }
  while(keyState != 'E'){
    // test
    readKBD();
    digitalWrite(buzzerPin, soundOn);
    
  }
  digitalWrite(buzzerPin, LOW);
  menuWrite(menuMainString);
  getNextAlarm(alarmFile);
}

//reads fileName from SD card without delimiter
//returns result as String built from individual characters from the SD card
String readFromSD(String fileName){
  Serial.println(String(F("ATTEMPTING CARD READ")));
  SD.begin(chipSelect);
  File readFile = SD.open(fileName);
  if(readFile){
    String res = "";
    while(readFile.available()){
      char nextChar = char(readFile.read());
      //EOF is reached
      if(nextChar == '#'){
        break;
      }
      res += nextChar;
    }
    readFile.close();
    return res;
  }
  else{
    Serial.println(String(F("Err: cannot open file (read): "))+fileName);
  }
}

//sets the time in tm and resets the second to 0
//these values are then sent to the RTC
bool setRTC(int newHour, int newMin){
  tmElements_t tm;
  tm.Hour = newHour;
  tm.Minute = newMin;
  tm.Second = 0;
  if (RTC.write(tm)) {
      return true;
    }
  else {
    return false;
  }
}

//returns a String in the format hh:MM
//leading zeros added to numbers n<10
String getTime(){
  tmElements_t tm;
  if(!RTC.read(tm)){
    if (RTC.chipPresent()) {
      //initialize clock
      if(!setRTC){
        Serial.println(String(F("ERR: RTC present but can't be set")));
      }
    } else {
      Serial.println(String(F("ERR: RTC not present")));
      return;
    }
  }
  else {
    //handle leading zeros and return
    String tempHour = String(tm.Hour);
    if (tm.Hour < 10) tempHour = '0'+tempHour;
    String tempMinute = String(tm.Minute);
    if (tm.Minute < 10) tempMinute = '0'+tempMinute;
    return tempHour+':'+tempMinute;
  }
}



///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/////////////////////// FUNCTIONS FOR STEPPER MOTOR   /////////////////////////////

void dispense(){
  // Set the spinning direction clockwise:
  digitalWrite(dirPin, HIGH);
  // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < (stepsPerRevolution)/8; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
}
