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

//variables
String nextAlarmTime = "";
String nextAlarmContent = "";

void setup() {
  Serial.begin(57600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);
  
  //setRTC(15,4);
  //getNextAlarm();
  /*
  char string1[] = "hello";
  char string2[] = "bonjour";
  char string3[] = "guten tag";

  char * strings[3];
  strings[0] = string1;
  Serial.println("meallanrum"+String(strings[0]));
  String test = "hello";
  test.toCharArray(strings[0], test.length()+1);
  Serial.println("meallanrum"+String(strings[0]));7
  */
}

void loop() {
  delay(1000);
  Serial.println("leggo");
  getAlarmInfo(alarmFile);
  //if(getTime() == nextAlarmTime) alarm();
  delay(100000);
}

//checks all alarms on SD card and selects the next alarm to be executed
//nextAlarmTime is set as next alarm and accompanying string is set in nextAlarmContent
/*
void getNextAlarm(char *pointer[]){
  nextAlarmTime = "23:59";
  for(int i = 0; i < *pointer.size(); i++ ) {
      //compare times
      String tempTime = *pointer[i];
      Serial.println(tempTime+" "+getTime()+" "+nextAlarmTime);
      if(tempTime > getTime() && tempTime < nextAlarmTime){ //"smallest" time AFTER current time
        nextAlarmTime = tempTime;
        nextAlarmContent = alarmString.substring(i+1,alarmString.substring(i+1).indexOf('&')+i+1);
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
*/

char * getAlarmInfo(char alarmfile[]){
  Serial.println("getAlarmInfo called");
  String alarmString = readFromSD(alarmFile);
  int nrOfLines = 0;
  for(int i = 0; i < alarmString.length(); i++ ) {
    if(alarmString[i] == ';'){
      nrOfLines += 1;
    }
  }
  Serial.println("nrOfLines: "+String(nrOfLines));
  char * pointer[nrOfLines];
  int index = 0;
  int count = 0;
  delay(1000);
  for(int i = 0; i < nrOfLines; i++ ) {
    count += 1;
    //set pointer of line i to content of line i in format 'hh:MM?ttt...t;'
    String sub = alarmString.substring(0, alarmString.indexOf(';'));//index
    Serial.println("sub:"+sub);
    Serial.println("whatever1:"+sub);
    String temp = sub;
    Serial.println("temp: "+temp);
    Serial.println("sub length: "+String(sub.length()));
    Serial.println("pointer: "+String(*pointer[i]));
    temp.toCharArray(pointer[i], sub.length());
    //Serial.println("whatever2:"+String(pointer[i]));
    alarmString = alarmString.substring(sub.length()+3);
  }
  Serial.println("count"+String(count));
  for(int i = 0; i < nrOfLines; i++ ) {
    Serial.println("mellanrum i: "+String(i));
    Serial.println("mellanrum"+String(*pointer[i]));
    //Serial.println("mellanrum"+type(pointer[i]));
  }
}

//activates the alarm and then calls getNextAlarm() after alarm has been deactivated
void alarm(){
  while(true){
   Serial.println("ALARM!");
   delay(1000);
  }
  //getNextAlarm();
}

//reads fileName from SD card without delimiter
//returns result as String built from individual characters from the SD card
String readFromSD(char fileName[]){
  Serial.println("ATTEMPTING CARD READ");
  SD.begin();
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
    Serial.println(res);
    delay(1000);
    return res;
  }
  else{
    Serial.println("Err: cannot open file (read)");
  }
}

//writes content+delimiter to fileName on SD card
//previous content is deleted
void writeToSD(String fileName, String content){
  content += '#';
  SD.begin();
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
