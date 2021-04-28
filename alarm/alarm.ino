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

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

  setRTC(15,4);
  getNextAlarm();
}

void loop() {
  if(getTime() == nextAlarmTime) alarm();
  delay(5000);
}

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
        nextAlarmContent = alarmString.substring(i+1,alarmString.substring(i+1).indexOf('&')+i+1);
      }
     }
     else if(alarmString[i] == '&'){
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
  while(true){
   Serial.println("ALARM!");
   delay(1000);
  }
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
  //tm.Second = 0;
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
