
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////////// SCREEN AND MENU SYSTEM ///////////////////////////////

#include <LiquidCrystal.h>

LiquidCrystal lcd(9,8,7,6,5,4); // generates an instance in the lcd
//LiquidCrystal lcd(12,11,5,4,3,2); // ORIGINAL CALL OF THE CLASS

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
int menuMain = 0;
int curMenuItem = 0;
String menuMainString[] = {"List alarms", "Set time", "Reset wheel", "Sound", "Shamoun!"};
String menuListAlarms[] = {"08:00", "12:30", "15:00", "18:45"};
int leng = 0;


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
String alarmFile = "alarms.txt";

//variable wires
const int chipSelect = 10;

//variables
String nextAlarmTime = "";
String nextAlarmContent = "";

//Buzzer
const int buzzerPin = 10;


// SCREEN SAVE
int counter = 0;

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
  //setRTC(15,4); // not needed anymore
  getNextAlarm();
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // INIT THE LCD AND MENU VARS
  lcd.begin(16,2);
  // calculate the lenght of the current menu array and write the initial menu
  leng = sizeof(menuMainString)/sizeof(menuMainString[0]);
  menuWrite(menuMainString);


  // INIT STEPPER MOTOR SYSTEM
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);


  //??? // TESTS
  setRTC(17, 4);
  getNextAlarm();
  //Alarm();

}

void loop() {


  //??? TESTS
  digitalWrite(buzzerPin, LOW);
  
  ///////////////////////////////////////////////////// LOOP MENU AND KBD
  readKBD(); // updates the keyState value
  
  if (counter % 4000 == 0) {
    String line1 = "Next alarm " + nextAlarmTime;
    myLCDprint(getTime(), line1);
    curMenuItem = 0;
  }
  
  
  if (keyState != prevKeyState && keyState != "N") {
    counter = 0;
    // calculate the lenght of the current menu array
    leng = sizeof(menuMainString)/sizeof(menuMainString[0]);
    
    //Serial.println(keyState);
    if (keyState == "U" && curMenuItem > 0) {
      curMenuItem -= 1;
      menuWrite(menuMainString);
      Serial.println("pressed: "+keyState);
    }
    if (keyState == "D" && curMenuItem < leng-1) {
      curMenuItem += 1;
      //Serial.println(curMenuItem);
      menuWrite(menuMainString);
      Serial.println("pressed: "+keyState);
    }
    
  }
  prevKeyState = keyState;

  counter ++;


  ///////////////////////////////////////////////////// LOOP SD AND ALARM
  //Serial.println("getTime: "+getTime()+" nextAlarmTime: "+nextAlarmTime);
  if(getTime() == nextAlarmTime) alarm();
  //delay(1000);
}



///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/////////////////////// FUNCTIONS FOR SCREEN AND MENU /////////////////////////////


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
  // Serial.println(kbdIn);
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


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/////////////////////// FUNCTIONS FOR SD, ALARM, CLOCK/////////////////////////////


//checks all alarms on SD card and selects the next alarm to be executed
//nextAlarmTime is set as next alarm and accompanying string is set in nextAlarmContent
void getNextAlarm(){
  String alarmString = readFromSD(alarmFile);
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
  Serial.println("#"+nextAlarmTime+"#");
  Serial.println("#"+nextAlarmContent+"#");
}

//activates the alarm and then calls getNextAlarm() after alarm has been deactivated
void alarm(){
  Serial.println("Alarm!");
  if(nextAlarmContent == "1"){
    dispense();
  } else {
    myLCDprint(nextAlarmContent, "");
  }
  while(keyState != "E"){
    readKBD();
    digitalWrite(buzzerPin, HIGH);
  }
  digitalWrite(buzzerPin, LOW);
  menuWrite(menuMainString);
  getNextAlarm();
}

//reads fileName from SD card without delimiter
//returns result as String built from individual characters from the SD card
String readFromSD(String fileName){
  Serial.println("ATTEMPTING CARD READ");
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
    Serial.println("Err: cannot open file (read): "+fileName);
  }
}

//writes content+delimiter to fileName on SD card
//previous content is deleted
void writeToSD(String fileName, String content){
  content += '#';
  SD.begin(chipSelect);
  //if file exists it is deleted to wipe content
  if(SD.exists(fileName)) SD.remove(fileName);
  File writeFile = SD.open(fileName, FILE_WRITE);
  if(writeFile){
    for(int i = 0; i < content.length(); i++ ) {
      writeFile.print(content[i]);
      //Making sure file isn't written to with duplicates, workaround for problem
      if(content[i] == '#') break;
    }
  }
  else {
    Serial.println("Err: cannot open file (write): "+fileName);
  }
  writeFile.close();
}

//sets the time in tm and resets the second to 0
//these values are then sent to the RTC
bool setRTC(int newHour, int newMin){
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

//gets the time from the RTC and updates tm
bool pollRTC(){
  if (RTC.read(tm)) {
    return true;
  }
  else return false;
}

//returns a String in the format hh:MM
//leading zeros added to numbers n<10
String getTime(){
  if(!pollRTC()){
    if (RTC.chipPresent()) {
      //initialize clock
      if(!setRTC){
        Serial.println("ERR: RTC present but can't be set");
      }
    } else {
      Serial.println("ERR: RTC not present");
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
