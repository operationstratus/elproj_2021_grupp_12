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

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

}

void loop() {
  checkAlarms();
  delay(5000);
}

//checks all recorded alarms against current time as reported by RTC
//doesn't work so well right now, better solution: split strings against delimiters and make arrays/structs
//also make use of something like nextAlarm to save processing power
//and to show to user
void checkAlarms(){
  String alarmString = readFromSD(alarmFile);
  String tempTime;
  for(int i = 0; i < alarmString.length(); i++ ) {
     if(alarmString[i] == '?'){
      //compare times
      tempTime.trim();
      Serial.println("Comp!"+tempTime+"!to!" +getTime()+"!");
      if(tempTime == getTime()){
        alarm();
        break;
      }
     }
     else if(alarmString[i] == '&'){
      tempTime = "";
     }
     else {
      tempTime += alarmString[i];
     }
  }
}

void alarm(){
  Serial.println("ALARM!");
  return;
}

//reads fileName from SD card without delimiter
//returns result as String
String readFromSD(String fileName){
  Serial.println("ATTEMPTING CARD READ");
  SD.begin(chipSelect);
  File readFile = SD.open(fileName);
  if(readFile){
    String res = "";
    while(readFile.available()){
      char nextChar = char(readFile.read());
      if(nextChar == '#'){
        Serial.println("EOF; breaking");
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

//Writes content+delimiter to fileName on SD card
void writeToSD(String fileName, String content){
  content += '#';
  SD.begin(chipSelect);
  SD.remove(fileName);
  File writeFile = SD.open(fileName, FILE_WRITE);
  if(writeFile){
    for(int i = 0; i < content.length(); i++ ) {
      writeFile.print(content[i]);
      if(content[i] == '#'){
        break;
      }
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
  tm.Second = 0;
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

//Returns a String in the format hh:MM
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
    return String(tm.Hour)+':'+String(tm.Minute);
  }
}
