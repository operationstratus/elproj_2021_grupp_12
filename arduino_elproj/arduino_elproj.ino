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
int kbdIn = 0; // pin for the keyboard
char keyState; // current key pressed: "E", "U", "D", "R", "L", or "N"
char prevKeyState; // key pressed in the last loop cycle

//-------------------------CALLING LCD PRINT
String line0(16); // string to be printed on the first row on the lcd screen
String line1(16); // string to be printed on the second row on the lcd screen

//-------------------------MENU SYSTEM
byte menuIntro = 0; // inital switch state of menuMain
byte curMenuArray = 0;
byte curMenuItem = 0;
char menuMainString[][11] = {"List alarms", "Set time", "Reset wheel", "Sound"};
char menuAlarmString[][11] = {"08:00", "12:30", "15:00", "18:45"};
char menuSoundString[][11] = {"Sound off", "Sound on"};
byte menuLeng = 0;
byte changedMenu = 0;


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

//time struct
tmElements_t tm;

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

void menuWrite(char menu[][11]) {
  //--------------------------------WRITE THE MENU
  //Serial.println(curMenuItem);
  byte index;
  for(byte i=0; i < 11; i++){
    if (menu[0+curMenuItem] != "") index = i;
  }
  String temp = String(menu[0+curMenuItem]).substring(0,index+1);
  line0 = ">" + temp;
  for(byte i=0; i < 11; i++){
    if (menu[1+curMenuItem] != "") index = i;
  }
  temp = String(menu[1+curMenuItem]).substring(0,index+1);
  if (curMenuItem < menuLeng-1){
    line1 = " " + temp;
  } else {
    line1 = " ";
  }

  //Serial.println("line0 = "+line0+"\nline1="+line1+"curMenuItem = "+curMenuItem+"\n\n");
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
        timeMenu();
        curMenuArray = 0;
        curMenuItem = 0;
        changedMenu = 0;
      break;

      case 4:
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


void timeMenu() {
  bool firstRun = true;
  byte tim[] = {0,0};
  byte modArr[] = {24, 60};
  byte menuIndex = 0;
  String line1(16), line2(16);
  while (true) {
    updateKBD();
    if (firstRun or keyState != prevKeyState){
      firstRun = false;
      if (menuIndex < 2) { //pointer is on hh or mm
        if(keyState == 'U'){
          tim[menuIndex] ++;
          tim[menuIndex] = tim[menuIndex]%modArr[menuIndex];//count up
        }
        if(keyState == 'D'){
          if (tim[menuIndex] != 0) {
            tim[menuIndex] --;
            tim[menuIndex] = tim[menuIndex]%modArr[menuIndex];//count down
          }
          else {
            tim[menuIndex] = modArr[menuIndex]-1;
          }
        }
      }
      if (keyState == 'R'){
        menuIndex += 1;
      }
      if (keyState == 'L' && menuIndex > 0){
        menuIndex -= 1;
      }
      String timeString; //build timeString
      if (tim[0]%modArr[0] < 10) timeString += '0';
      timeString += String(tim[0])+':';
      if (tim[1]%modArr[1] < 10) timeString += '0';
      timeString += String(tim[1]);
      
      if (menuIndex == 3) { //user pressed R when on OK
        setRTC(tim[0], tim[1]);
        break;
      }
      if (keyState != 'N') { //only update LCD if button has been pressed
        line1 = timeString+String(F(" OK"));
        line2 = "";
        for (byte i = 0; i < menuIndex; i+=1) {
          for (byte j = 0; j < 3; j+=1) {
            line2 += ' ';
          }
        }
        line2 = line2 + '^' + '^';
        printLCD(line1, line2);
      }
      prevKeyState = keyState;
    }
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
