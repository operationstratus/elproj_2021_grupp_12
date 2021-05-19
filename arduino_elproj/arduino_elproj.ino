//sebTest
////////////////////////////////////////
////////////////////////////////////////
//////////////////////        LIBRARIES

#include <LiquidCrystal.h>
LiquidCrystal lcd(9, 8, 7, 6, 5, 4); // generates an instance in the lcd

//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////    GLOBAL VARIABLES

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
//menu items are all 16 items long to account for the longest item in char array
char menuMainString[][10] = {"List alarms", "Set time", "Sound"};
//char menuMainString[][16] = {"List alarms", "Set time", "Reset wheel", "Sound"};
char menuAlarmString[7][10];
//max number of alarms per day is set here at 7.
//Max number of chars in each slot is 16, screen width
//char menuSoundString[][16] = {"Sound off", "Sound on"};
byte menuLeng = 0;
bool changedMenu = false;

//////////////////////////////////////////////
//////////////////////////////////////////////
////////////////////// CLOCK, SD READER, ALARM
// include the SD library:
#include <SPI.h>
#include <SD.h>
//include RTC
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

//files and content
char alarmFile[] = "alarms.txt";
String alarmString(200);

//variables
String nextAlarmTime(6); //6 chars since it is hh:MM and exit character
String nextAlarmContent(25); //12*2 (LCD characters) + exit character

//Buzzer
const int buzzerPin = 10;

// SCREEN SAVE
const byte screenSaverTime = 50; // how many loop cyckles before screen enters screen saver
int counterScreen = screenSaverTime; // makes sure that the screensaver is shown on boot

//varför använder vi inte bara counterScreen??
// CHECKING THE USERS ALARMS WITH CERTAIN INTERVALS
const byte AlarmCheckTime = 100; // how many loop cyckles between each time the machine checks and updates the list of alarms
int counterAlarmCheck = 0; // counter of keeping track of this

// ALARM
bool soundOn = true;


//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////       STEPPER MOTOR

// Define stepper motor connections and steps per revolution:
#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200*2 // times two since we have enabled half stepping
#define stepDelay 5000
// KEEPING TRACK OF HOW MANY DISPENSES
byte dispenseCount = 1;
bool needToRefill = false; // varför? om dispenseCount = 0 är det överflödigt





//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////          SETUP



void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);
  alarmString = readFromSD(alarmFile); //this is only needed on boot, allocate the alarmSTring memory early and keep it
  //Serial.println(String(alarmString));
  updateAlarmList(alarmString); //only needed on boot, never changes

  updateAlarmNext();

  // INIT STEPPER MOTOR SYSTEM
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // INIT BUZZER SYSTEM
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // INIT THE LCD AND MENU VARS
  lcd.begin(16, 2);
  //user welcome message on boot
  printLCD(String(F("   WELCOME TO")), String(F("    ROSETTEN")));

  //show the welcome message for some time
  delay(2500);
}


//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////        LOOP
void loop() {


  updateKBD();

  //update menu, but only if not have to refill:
  if (needToRefill) {
    if (keyState != prevKeyState and keyState != 'N') {
      // if need to refill, press any key to turn off refill alert message
      needToRefill = false;
      dispenseCount = 1;
      changedMenu = true; // this ensures that the menu gets update

    }
  } else {
    updateMenu();
  }

  // UPDATES AT END OF LOOP

  //--------------------------if 14 dispences has been made, then set needToRefill to true witch puts the machine into refill alert mode
  if (dispenseCount == 14) {
    Serial.println(String(F("EMPTY")));
    needToRefill = true;
    printLCD(String(F("     ")) + getTime(), String(F(" EMPTY, REFILL! ")));
  }


  // check if its time to check and update alarm
  if (counterAlarmCheck < AlarmCheckTime) {
    counterAlarmCheck++;
  } else {
    counterAlarmCheck = 0;
    if (getTime() == nextAlarmTime) {
      // checks if alarm() should be called
      alarm();
    }
    updateAlarmNext();
  }


  if (counterScreen < screenSaverTime) counterScreen++;
  prevKeyState = keyState;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////      FUNCTIONS

//supposed to be empty? Eller är det header för kommande?


// FUNC MENU

void menuWrite(char menu[][10]) {
  //--------------------------------WRITE THE MENU
  //Serial.println(curMenuItem);
  byte index;
  for (byte i = 0; i < 10; i++) {
    if (menu[0 + curMenuItem] != "") index = i;
  }
  String temp = String(menu[0 + curMenuItem]).substring(0, index + 1);
  line0 = '>' + temp;
  if (curMenuItem < menuLeng - 1) {
    for (byte i = 0; i < 10; i++) {
      if (menu[1 + curMenuItem] != "") index = i;
    }
    temp = String(menu[1 + curMenuItem]).substring(0, index + 1);
    line1 = ' ' + temp;
  } else {
    line1 = ' ';
  }
  printLCD(line0, line1);
}

//prints the two Strings Line0 and Line1 to the LCD display in order
void printLCD(String Line0, String Line1) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(Line0);
  lcd.setCursor(0, 1);
  lcd.print(Line1);
}

// FUNC KBD

//reads the analog value of the kbdPin to determin which button is pressed
//assigns keyState a char value according to the kbdPin read value
void updateKBD() {
  int kbdIn = analogRead(kbdPin);
  //Serial.println("kbdIn = "+String(kbdIn)); //debug

  //Configuration:
  //               Up
  // Enter    Left    Right
  //              Down
  // --------------------------
  // Analog value of output:
  // None: 1023 (can vary a little, range is implemented)
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
  } else if (kbdIn >= 1000) {
    keyState = 'N';
  }
  delay(100);
}

//updates the menu with screensaver if user inactive and facilitates the selection of items in the menu
void updateMenu() {
  //---------------------------------------SCREEN SAVER
  if (counterScreen == screenSaverTime) {
    if (dispenseCount < 10) {
      printLCD(String(getTime() + F("  Dose ") + dispenseCount + F("/14")), String(F("Next Alarm ")) + nextAlarmTime);
    } else {
      printLCD(String(getTime() + F("  Dose") + dispenseCount + F("/14")), String(F("Next Alarm ")) + nextAlarmTime);
    }
    curMenuItem = 0;
  }


  if (changedMenu or (keyState != prevKeyState and keyState != 'N')) {

    // if this was entered from screen saver mode:
    if (counterScreen == screenSaverTime) {
      counterScreen = 0;
      curMenuItem = 0;
      menuLeng = sizeof(menuMainString) / sizeof(menuMainString[0]); // calculate the length of a menuArray
      menuWrite(menuMainString);
      prevKeyState = keyState;

    } else {
      counterScreen = 0; // reset screen time counterScreen

      switch (curMenuArray) {
        case 0:

          menuLeng = sizeof(menuMainString) / sizeof(menuMainString[0]); // calculate the length of a menuArray

          // IF JUST CHANGED A MENU, DO THIS INSTEAD OF WHAT COMES BELOW THIS IF-STATEMENT
          if (changedMenu) {

            menuWrite(menuMainString);
            changedMenu = false;
            break;
          }

          if (keyState == 'U' && curMenuItem > 0) {
            curMenuItem--;
            menuWrite(menuMainString);
          } else if (keyState == 'D' && curMenuItem < menuLeng - 1) {
            curMenuItem++;
            menuWrite(menuMainString);
          } else if (keyState == 'R') {

            curMenuArray = curMenuItem + 1;
            curMenuItem = 0;
            changedMenu = true;
          } else {
            menuWrite(menuMainString);
          }
          break;

        case 1:
          menuLeng = sizeof(menuAlarmString) / sizeof(menuAlarmString[0]); // calculate the length of a menuArray

          // IF JUST CHANGED A MENU, DO THIS INSTEAD OF WHAT COMES BELOW THIS IF-STATEMENT
          if (changedMenu) {
            menuWrite(menuAlarmString);
            changedMenu = false;
            break;
          }



          if (keyState == 'U' && curMenuItem > 0) {
            curMenuItem--;
            menuWrite(menuAlarmString);
          } else if (keyState == 'D' && curMenuItem < menuLeng - 1) {
            curMenuItem++;
            menuWrite(menuAlarmString);
          } else if (keyState == 'L') {
            curMenuArray = 0;
            curMenuItem = 0;
            changedMenu = true;
          } else {
            menuWrite(menuAlarmString);
          }
          break;

        case 2: //setTime()
          changedMenu = true;//enters timeMenu() with an update before button is pressed
          timeMenu();
          updateAlarmNext(); // sebTest bad?
          curMenuArray = 0;
          curMenuItem = 0;
          menuLeng = sizeof(menuMainString) / sizeof(menuMainString[0]); // calculate the length of a menuArray
          menuWrite(menuMainString);
          break;

        case 3: //sound setting, toggles soundOn and beeps if soundOn is now true
          soundOn = !(soundOn);
          if (soundOn == true) {
            digitalWrite(buzzerPin, HIGH);
            delay(500);
            digitalWrite(buzzerPin, LOW);
          }
          curMenuArray = 0;
          curMenuItem = 0;
          menuWrite(menuMainString);
          break;
      }
    }
  }
}

//allows the user to set the time in an interactive menu
void timeMenu() {
  byte tim[] = {0, 0};
  byte modArr[] = {24, 60};
  byte menuIndex = 0;
  String line1(16), line2(16);
  while (true) {
    updateKBD();
    bool buttonNotPressed = true;
    if (changedMenu or keyState != prevKeyState) {
      if (menuIndex < 2 && buttonNotPressed) { //pointer is on hh or mm
        if (keyState == 'U') {
          tim[menuIndex] ++; //??? kanske kan göras på en rad?
          tim[menuIndex] = tim[menuIndex] % modArr[menuIndex]; //count up
          buttonNotPressed = false;
        }
        if (keyState == 'D' && buttonNotPressed) {
          if (tim[menuIndex] != 0) {
            tim[menuIndex] --;
            tim[menuIndex] = tim[menuIndex] % modArr[menuIndex]; //count down
            buttonNotPressed = false;
          }
          else { //handles wrapping around so that 0--; --> 23 || 59 not 255%(23 || 59)
            tim[menuIndex] = modArr[menuIndex] - 1;
          }
        }
      }
      if (keyState == 'R' && buttonNotPressed and not changedMenu) {
        menuIndex += 1;
        buttonNotPressed = false;
      }
      if (keyState == 'L' && menuIndex > 0 && buttonNotPressed) {
        menuIndex -= 1;
      }
      String timeString; //build timeString
      if (tim[0] % modArr[0] < 10) timeString += '0';
      timeString += String(tim[0]) + ':';
      if (tim[1] % modArr[1] < 10) timeString += '0';
      timeString += String(tim[1]);

      if (menuIndex == 3) { //user pressed R when on OK (pointer on OK)
        setRTC(tim[0], tim[1]);
        break;
      }
      if (keyState != 'N' or changedMenu) { //only update LCD if button has been pressed or menu just entered
        line1 = timeString + String(F(" OK"));
        line2 = "";
        for (byte i = 0; i < menuIndex; i += 1) {
          for (byte j = 0; j < 3; j += 1) {
            line2 += ' ';
          }
        }
        line2 = line2 + '^' + '^';//2 characters to avoid String()
        printLCD(line1, line2);
      }
      prevKeyState = keyState;
      changedMenu = false;
    }
  }
}
//////////////////////////////////////////////
//////////////////////////////////////////////
/////////////  FUNCTIONS FOR SD, ALARM, CLOCK

//checks all alarms on SD card and selects the next alarm to be executed
//nextAlarmTime is set as next alarm and accompanying string is set in nextAlarmContent

void updateAlarmNext(){
  String tempTime(5);
  tempTime = ""; // for some reason, tempTime is initially set to "5"
  nextAlarmTime = "23:59";
  for(int i = 0; i < alarmString.length(); i++) {
     if(alarmString[i] == '?'){
      //compare times
      tempTime.trim();
      Serial.println(tempTime+" "+getTime()+" "+nextAlarmTime);
      if(tempTime > getTime() && tempTime < nextAlarmTime){ //"smallest" time AFTER current time
        nextAlarmTime = tempTime;
        nextAlarmContent = alarmString.substring(i+1,alarmString.substring(i+1).indexOf('\n')+i+1);
        Serial.println(String(F("nextAlarmContent = "))+nextAlarmContent+String(F(", nextAlarmTime = "))+String(nextAlarmTime));
      }
     }
     else if(alarmString[i] == '\n'){
      tempTime = "";
     }
     else {
      tempTime += alarmString[i];
     }
  }
}

//???

/*
void updateAlarmNext() {
  String tempTime(5); // temporary var for time
  nextAlarmTime = "23:59"; // initial value

  menuLeng = sizeof(menuAlarmString) / sizeof(menuAlarmString[0]); // calculate the length of a menuArray
  for (int i = 0; i < menuLeng; i++) {

    String tempString = String(menuAlarmString[i]);
    // set tempString as the string in the current slot of menuAlarmString
    tempString.trim();
    // without any spaces before and after

    if (tempString.length() > 0) {
      tempTime = tempString.substring(0, 5); // time xx:xx is the first 5 chars

      if (tempTime > getTime() && tempTime < nextAlarmTime) { //"smallest" time AFTER current time
        nextAlarmTime = tempTime;
        nextAlarmContent = tempString.substring(5); // content is everything after the time

      }
    }
  }
}
*/
/*
  void updateAlarmList(String alarmString) {
  byte nrOfLines = 0;
  for(byte i = 0; i < alarmString.length(); i++ ) {
    if(alarmString[i] == ';'){
      nrOfLines ++;
    }
  }
  byte index = 0;
  for(byte i = 0; i < nrOfLines; i++) {
    //alarmString.substring(index, alarmString.substring(index).indexOf(';')).toCharArray(menuAlarmString[i], 11);
    alarmString.substring(index, 4).toCharArray(menuAlarmString[i], 11); //get hh:MM of each line
    index += 5;
    if (alarmString.charAt(index) == '1'){ //a dose is represented by a D in the list
      menuAlarmString[i][5] = 'D';
    }
    else {
      menuAlarmString[i][5] = 'M';
    }
    index += alarmString.substring(index).indexOf(';'); //set index to the beginning of next line
  }

  //debug, print menuAlarmString
  for(byte i = 0; i<sizeof(menuAlarmString); i++){
    for(byte j = 0; j<16; j++)  {
      if (menuAlarmString[i][j] != ""){
        Serial.print(menuAlarmString[i][j]);
      }
    }
  }
  }

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void updateAlarmList(String alarmString) {
  byte nrOfLines = 0; // temporary variable nrOfLines, to calc how many rows we have in alarmString
  for (byte i = 0; i < alarmString.length(); i++ ) {
    // for every character in alarmString:
    if (alarmString[i] == '\n') { // if this character is ';'
      nrOfLines ++;
      //Serial.println(String(F("nrOfLines = "+String(nrOfLines))));

    }
  }

  byte sumIndex = 0;
  for (byte i = 0; i < nrOfLines; i++) {
    // for i in the range 0 to nrOfLines-1

    byte indexOfNextEndOfLine = alarmString.substring(sumIndex).indexOf('\n') + sumIndex;
    // index of the first newline symbol in the substring from sumIndex
    
    String stringToLoad = alarmString.substring(sumIndex, indexOfNextEndOfLine);
    if (stringToLoad[6] == '1') {
      stringToLoad = stringToLoad.substring(0, 5) + String(F(" DOSE"));
    } else {
      stringToLoad = stringToLoad.substring(0, 5) + String(F(" MSG"));
    }
    // stringToLoad is the substring between 0 and next newline symbol
    //Serial.println(String(F("    stringToLoad = ")) + stringToLoad);

    stringToLoad.toCharArray(menuAlarmString[i], 10);
    // insert stringToLoad in place i in menuAlarmString array, has to specify slot size (16)

    sumIndex += (indexOfNextEndOfLine - sumIndex)+1;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//activates the alarm and then calls getNextAlarm() after alarm has been deactivated
void alarm() {
  Serial.println(String(F("Alarm!")));
  if (nextAlarmContent == "1") {
    printLCD(String(F("TAKE YOUR MEDS!")), String(F("")));
    dispense();
  } else {
    //??? FIXA, SKA TA IN nextAlarmContent inte hårdkodat
    printLCD(nextAlarmContent.substring(0, 16), nextAlarmContent.substring(16, 32));
  }

  digitalWrite(buzzerPin, soundOn);
  while (keyState == 'N') {
    updateKBD();
    Serial.println(String(F("Press any key to exit alarm!")));
    delay(50);
  }
  digitalWrite(buzzerPin, LOW);
  Serial.println(String(F("This should lead us back to main menu")));
  curMenuArray = 0;
  curMenuItem = 0;
  changedMenu = true;
  
  updateAlarmNext();
  delay(200);
}

//reads fileName from SD card without delimiter
//returns result as String built from individual characters from the SD card output stream
String readFromSD(String fileName) {
  Serial.println(String(F("ATTEMPTING CARD READ")));
  SD.begin();
  File readFile = SD.open(fileName);
  if (readFile) {
    bool reading = true;
    //??? cap maximum string length by allocating size on declaration? Eller bara hoppas att det återvinns
    String res = "";
    while (readFile.available() and reading == true) {
      char nextChar = char(readFile.read());
      //EOF is reached
      if (nextChar == '#') {
        Serial.println(String(F("COMPLETED CARD READ")));
        readFile.close();
        reading = false;
        break;
      }
      res += nextChar;
    }
    readFile.close();
    return res;
  }
  else {
    Serial.println(String(F("Err: cannot open file (read): ")) + fileName);
  }
}

//sets the time and resets the second to 0
//these values are set to the newly created tmElements instance then sent to the RTC
//when a value is <10 and passed to RTC returning string will miss leading 0, workaround in getTime()
bool setRTC(int newHour, int newMin) {
  tmElements_t tm;
  tm.Hour = newHour;
  tm.Minute = newMin;
  tm.Second = 50;
  if (RTC.write(tm)) {
    return true;
  }
  else {
    return false;
  }
}

//returns a String in the format hh:MM pulled from tmElements
//leading zeros added to numbers n<10
String getTime() {
  tmElements_t tm;
  if (!RTC.read(tm)) {
    if (RTC.chipPresent()) {
      //initialize clock
      if (!setRTC) {
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
    if (tm.Hour < 10) tempHour = '0' + tempHour;
    String tempMinute = String(tm.Minute);
    if (tm.Minute < 10) tempMinute = '0' + tempMinute;
    return tempHour + ':' + tempMinute;
  }
}


//////////////////////////////////////////////
//////////////////////////////////////////////
////////////////  FUNCTIONS FOR STEPPER MOTOR

//dispenses one container of pills
void dispense() {
  Serial.println(String(F("Dispensing")));
  // Set the spinning direction counterclockwise:
  digitalWrite(dirPin, LOW);
  // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < (stepsPerRevolution) / 8; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  if (dispenseCount < 14) dispenseCount++;
}
