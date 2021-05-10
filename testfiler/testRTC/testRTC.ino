/* Testfil för att skriva ut tiden i terminalen
 * Kopplingar till RTC:
 * VCC-->5V
 * SDA-->A4
 * SCL-->A5
 */

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

tmElements_t tm;

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

  //setRTC(10,10);d
  
}

void loop() {
  String curTime = getTime();
  Serial.println(curTime);
  delay(1000);
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
