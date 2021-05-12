
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////////// SCREEN AND MENU SYSTEM ////////////////////////////////

#include <LiquidCrystal.h>

LiquidCrystal lcd(9,8,7,6,5,4); // generates an instance in the lcd
//LiquidCrystal lcd(12,11,5,4,3,2); // ORIGINAL CALL OF THE CLASS

//-------------------------READING KEYBOARD
const byte kbdPin = A0;
int kbdIn = 0; // pin for the keyboard
char returnStr; // variable used inside readKBD() function
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
byte leng = 0;


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
//tmElements_t tm;

//files
char alarmFile[] = "alarms.txt";

//variable wires
const byte chipSelect = 10;

//variables
String nextAlarmTime = "";
String nextAlarmContent = "";

//Buzzer
const byte buzzerPin = 10;


// SCREEN SAVE
int counter = 0;


// ALARM
byte soundOn = 1;


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////       STEPPER MOTOR     ///////////////////////////////

/*Example sketch to control a stepper motor with A4988/DRV8825 stepper motor driver and Arduino without a library. More info: https://www.makerguides.com */
// Define stepper motor connections and steps per revolution:
#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200*2 // times two since we have enabled half stepping
#define stepDelay 5000

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////        SETUP        ///////////////////////////////

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

  // INIT THE ALARM AND SD
  //setRTC(15,4); // not needed anymore
  //getAlarmInfo(alarmFile);
  //getNextAlarm();
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
  setRTC(19, 4);
  getNextAlarm(alarmFile);
  //Alarm();

  timeMenu();

}

void loop() {


  //??? TESTS
  digitalWrite(buzzerPin, LOW);

  updateMenu();
  


  ///////////////////////////////////////////////////// LOOP SD AND ALARM
  //Serial.println("getTime: "+getTime()+" nextAlarmTime: "+nextAlarmTime);
  if(getTime() == nextAlarmTime) alarm();
  //delay(1000);

  //Serial.println(String(F("Free ram: "))+String(freeMemory()));
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/////////////////////// FUNCTIONS FOR SCREEN AND MENU /////////////////////////////


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
  if (curMenuItem < leng-1){
    line1 = " " + temp;
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
  //Serial.println(kbdIn);
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
    returnStr = 'E';
  } else if (164 < kbdIn && kbdIn < 175) {
    returnStr = 'L';
  } else if (0 <= kbdIn && kbdIn < 10) {
    returnStr = 'R';
  } else if (85 < kbdIn && kbdIn < 95) {
    returnStr = 'U';
  } else if (25 < kbdIn && kbdIn < 40) {
    returnStr = 'D';
  } else if (kbdIn == 1023) {
    returnStr = 'N';
  }
  keyState = returnStr;
}


void updateMenu() {
  ///////////////////////////////////////////////////// LOOP MENU AND KBD
  readKBD(); // updates the keyState value
  if (counter % 400 == 0) {
    String line1 = String(F("Next alarm ")) + nextAlarmTime;
    myLCDprint(getTime(), line1);
    curMenuItem = 0;
  }
  //Serial.println("curMenuArray = "+ String(curMenuArray));
  switch(curMenuArray) {
    case 0:
    if (keyState != prevKeyState && keyState != 'N'){
      mainMenu();
    }
    break;
    case 1:
    if (keyState != prevKeyState && keyState != 'N') {
      alarmMenu();
    }
    break;
    case 2:
    if (keyState != prevKeyState && keyState != 'N') {
      soundMenu();
    }
    break;
  }
  prevKeyState = keyState;
  counter ++;
}

void mainMenu() {
  menuWrite(menuMainString);
    counter = 0;
    // calculate the lenght of the current menu array
    leng = sizeof(menuMainString)/sizeof(menuMainString[0]);
    
    //Serial.println(keyState);
    if (keyState == 'U' && curMenuItem > 0) {
      curMenuItem -= 1;
      menuWrite(menuMainString);
      Serial.println(String(F("pressed: "))+keyState);
    } else if (keyState =='D' && curMenuItem < leng-1) {
      curMenuItem += 1;
      //Serial.println(curMenuItem);
      menuWrite(menuMainString);
      Serial.println(String(F("pressed: "))+keyState);
    } else if (keyState == 'R') {
      Serial.println(String(F("pressed: "))+keyState);
      curMenuArray = curMenuItem+1;
      curMenuItem = 0;
      keyState = 'N';
      updateMenu();
    }
}
void alarmMenu() {
  counter = 0;
  menuWrite(menuAlarmString);
  // calculate the lenght of the current menu array
  leng = sizeof(menuAlarmString)/sizeof(menuAlarmString[0]);
  
  //Serial.println(keyState);
  if (keyState == 'U' && curMenuItem > 0) {
    curMenuItem -= 1;
    menuWrite(menuAlarmString);
  }
  if (keyState == 'D' && curMenuItem < leng-1) {
    curMenuItem += 1;
    menuWrite(menuAlarmString);
    Serial.println(String(F("pressed: "))+keyState);
  }
  if (keyState == 'E' || keyState == 'L') {
    Serial.println(String(F("pressed: "))+keyState);
    curMenuArray = 0;
    curMenuItem = 0;
    updateMenu();
    exit;
  }
}

void soundMenu() {
  
  Serial.println(String(F("NUUUUUUU IIIIIII SOOOOOUUUND, keystate="))+keyState);

  counter = 0;
  menuWrite(menuSoundString);
  // calculate the lenght of the current menu array
  leng = sizeof(menuSoundString)/sizeof(menuSoundString[0]);
  
  //Serial.println(keyState);
  if (keyState == 'U' && curMenuItem > 0) {
    curMenuItem -= 1;
    menuWrite(menuSoundString);
  } else if (keyState == 'D' && curMenuItem < leng-1) {
    curMenuItem += 1;
    menuWrite(menuSoundString);
    Serial.println(String(F("pressed: "))+keyState);
  } else if (keyState == 'E') {
    Serial.println(String(F("pressed: "))+keyState);
    soundOn = curMenuItem;
    Serial.println(String(F("soundOn = "))+String(soundOn));
    curMenuArray = 0;
    curMenuItem = 0;
    updateMenu();
    exit;
  } else if (keyState == 'L') {
    Serial.println(String(F("pressed: "))+keyState);
    curMenuArray = 0;
    curMenuItem = 0;
    updateMenu();
    exit;
  }
}

void timeMenu() {
  byte tim[] = {0,0};
  byte modArr[] = {24, 60};
  byte menuIndex = 0;
  String line1(16), line2(16);
  while (true) {
    readKBD();
    if (keyState != prevKeyState){
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
      myLCDprint(line1, line2);
    }
    prevKeyState = keyState;
  }}
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
/*
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

/* not used
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
*/
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
/*
//gets the time from the RTC and updates tm
bool pollRTC(){
  if (RTC.read(tm)) {
    return true;
  }
  else return false;
}
*/
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
