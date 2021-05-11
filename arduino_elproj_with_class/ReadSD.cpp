#include "Arduino.h"
#include "ReadSD.h"


/////////////////////////////////////////// libraries for ReadSD
#include <SPI.h>
#include <SD.h>
#include <DS1307RTC.h>


//files
String alarmFile = "alarms.txt";

// ALARM
String _nextAlarmTime = "";
String _nextAlarmContent = "";

//time struct
tmElements_t tm;


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

void ReadSD::getNextAlarm(){

  
  String SDString = readFromSD(alarmFile);
  String tempTime = "";
  String _nextAlarmTime = "23:59"; // last possible time
  String nextAlarmContent = "";

  
  for(int i = 0; i < SDString.length(); i++ ) {
     if(SDString[i] == '?'){
      //compare times
      tempTime.trim();

      if(tempTime > getTime() && tempTime < _nextAlarmTime){ //"smallest" time AFTER current time
        _nextAlarmTime = tempTime;
        _nextAlarmContent = SDString.substring(i+1,SDString.substring(i+1).indexOf(';')+i+1);
        i += sizeof(_nextAlarmContent);
      }
     }
     else if(SDString[i] == ';'){
      tempTime = "";
     }
     else {
      tempTime += SDString[i];
     }
  }
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// GETTERS ///////////////////////////////////////
String ReadSD::getNextAlarmTime() {
  return _nextAlarmTime;
}

String ReadSD::getNextAlarmContent() {
  return _nextAlarmContent;
}



bool ReadSD::setRTC(int newHour, int newMin){

  //sets the time in tm and resets the second to 0
  //these values are then sent to the RTC

  
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


//Returns a String in the format hh:MM
String ReadSD::getTime(){
  RTC.read(tm);
  return String(tm.Hour)+':'+String(tm.Minute);
}
