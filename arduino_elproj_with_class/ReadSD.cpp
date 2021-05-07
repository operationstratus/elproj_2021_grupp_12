#include "Arduino.h"
#include "ReadSD.h"


/////////////////////////////////////////// libraries for ReadSD
#include <SPI.h>
#include <SD.h>
#include <DS1307RTC.h>


//files
String alarmFile = "alarms.txt";


ReadSD::ReadSD(int chipSelect)
{
  _chipSelect = chipSelect;
}


String ReadSD::readFromSD(String fileName){
  //reads fileName from SD card without delimiter
  //returns result as String built from individual characters from the SD card
  Serial.println("ATTEMPTING CARD READ");
  SD.begin(_chipSelect);
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


String ReadSD::getNextTime(){
  String alarmString = readFromSD(alarmFile);
  String tempTime = "";
  nextAlarmTime = "23:59";
  Serial.println(alarmString);
  for(int i = 0; i < alarmString.length(); i++ ) {
     if(alarmString[i] == '?'){
      //compare times
      tempTime.trim();
      //Serial.println(tempTime+" "+getTime()+" "+nextAlarmTime);
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
  return nextAlarmTime
}


String ReadSD::getNextString(){
  String rtn = "Hello world!";
  return rtn;
}
